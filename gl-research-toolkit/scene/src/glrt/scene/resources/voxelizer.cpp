#include <glrt/scene/resources/voxelizer.h>
#include <glrt/scene/resources/voxel-file.h>
#include <glrt/scene/resources/asset-converter.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/static-mesh-file.h>
#include <glrt/scene/resources/texture-file.h>
#include <glrt/toolkit/geometry.h>

#include "cpuvoxelizerimplementation.h"

namespace glrt {
namespace scene {
namespace resources {

namespace Voxelizer_Private {

QHash<Uuid<StaticMesh>, QSet<CoordFrame>> staticMeshesToVoxelize_SingleSided;
QHash<Uuid<StaticMesh>, QSet<CoordFrame>> staticMeshesToVoxelize_TwoSided;
QHash<Uuid<StaticMesh>, Uuid<StaticMesh>> staticMeshesToVoxelize_Proxies;
QSet<Uuid<StaticMesh>> staticMeshesToVoxelize_all;
Uuid<StaticMesh> staticMeshesToVoxelize_anchorMesh;
CoordFrame staticMeshesToVoxelize_anchorFrame;

} // namespace Voxelizer_Private

using namespace Voxelizer_Private;

using AngelScriptIntegration::AngelScriptCheck;
using utilities::GlTexture;


Voxelizer::Implementation* Voxelizer::Implementation::singleton = nullptr;

VoxelFile::MetaData voxelizeImplementation(const QFileInfo& targetTextureFileName, const TriangleArray& staticMesh, size_t meshDataSize, Voxelizer::FieldType type, Voxelizer::MeshType meshType, const Voxelizer::Hints& hints);


QString Voxelizer::toString(MeshType meshType)
{
  switch(meshType)
  {
  case MeshType::TWO_SIDED:
    return "TWO_SIDED";
  case MeshType::FACE_SIDE:
    return "FACE_SIDE";
  case MeshType::MANIFOLD_RAY_CHECK:
  default:
    return "MANIFOLD_RAY_CHECK";
  }
}

QString Voxelizer::toAngelScript(MeshType meshType)
{
  return QString("VoxelMeshType::%0").arg(toString(meshType));
}

Voxelizer::Voxelizer(ResourceIndex* resourceIndex)
  : resourceIndex(resourceIndex)
{
}

Voxelizer::~Voxelizer()
{
}

void Voxelizer::registerAngelScriptAPI()
{
  int r;
  const char* nameVoxelizer = "Voxelizer";
  const char* nameHints = "VoxelizerHints";

  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterEnum("VoxelFieldType");
  r = angelScriptEngine->RegisterEnumValue("VoxelFieldType", "SIGNED_DISTANCE_FIELD", int(FieldType::SIGNED_DISTANCE_FIELD)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterEnum("VoxelMeshType");
  r = angelScriptEngine->RegisterEnumValue("VoxelMeshType", "FACE_SIDE", int(MeshType::FACE_SIDE)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("VoxelMeshType", "MANIFOLD_RAY_CHECK", int(MeshType::MANIFOLD_RAY_CHECK)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("VoxelMeshType", "TWO_SIDED", int(MeshType::TWO_SIDED)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType(nameHints, sizeof(Hints), AngelScript::asOBJ_VALUE|AngelScript::asOBJ_POD|AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour(nameHints, AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_constructor<Hints>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameHints, "TextureType voxelType", asOFFSET(Hints,voxelType)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameHints, "float extend", asOFFSET(Hints,extend)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameHints, "int minSize", asOFFSET(Hints,minSize)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameHints, "int maxSize", asOFFSET(Hints,maxSize)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameHints, "float scaleFactor", asOFFSET(Hints,scaleFactor)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType(nameVoxelizer, sizeof(Voxelizer), AngelScript::asOBJ_VALUE|AngelScript::asOBJ_POD|AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour(nameVoxelizer, AngelScript::asBEHAVE_CONSTRUCT, "void f(ResourceIndex@ index)", AngelScript::asFUNCTION((&AngelScriptIntegration::wrap_constructor<Voxelizer, ResourceIndex*>)), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameVoxelizer, "ResourceIndex@ resourceIndex", asOFFSET(Voxelizer,resourceIndex)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameVoxelizer, "VoxelizerHints signedDistanceField", asOFFSET(Voxelizer,signedDistanceField)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(nameVoxelizer, "void voxelize(const Uuid<StaticMesh> &in staticMeshUuid, VoxelMeshType meshType=VoxelMeshType::FACE_SIDE)", AngelScript::asMETHODPR(Voxelizer,voxelize,(const Uuid<StaticMesh>&, MeshType),void), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(nameVoxelizer, "void voxelize(const Uuid<StaticMesh> &in staticMeshUuid, VoxelMeshType meshType, const Uuid<StaticMesh> &in proxyStaticMesh)", AngelScript::asMETHODPR(Voxelizer,voxelize,(const Uuid<StaticMesh>&, MeshType,const Uuid<StaticMesh>&),void), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(nameVoxelizer, "void beginGroup()", AngelScript::asMETHOD(Voxelizer,beginJoinedGroup), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(nameVoxelizer, "void addToGroup(const Uuid<StaticMesh> &in staticMeshUuid, const CoordFrame &in coordFrame=CoordFrame(), bool two_sided=false)", AngelScript::asMETHODPR(Voxelizer,addToGroup,(const Uuid<StaticMesh>& meshUuid, const CoordFrame& frame, bool two_sided),void), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(nameVoxelizer, "void addToGroup(const Uuid<StaticMesh> &in staticMeshUuid, const CoordFrame &in coordFrame, bool two_sided, const Uuid<StaticMesh> &in proxyMeshUuid)", AngelScript::asMETHODPR(Voxelizer,addToGroup,(const Uuid<StaticMesh>& meshUuid, const CoordFrame& frame, bool two_sided, const Uuid<StaticMesh>& proxyMesh),void), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(nameVoxelizer, "void voxelizeGroup(VoxelMeshType meshType=VoxelMeshType::FACE_SIDE)", AngelScript::asMETHOD(Voxelizer,voxelizeJoinedGroup), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

Voxelizer::FileNames::FileNames(ResourceIndex* resourceIndex, const Uuid<StaticMesh>& staticMeshUuid, const Uuid<StaticMesh>& proxyStaticMeshUuid)
  : staticMeshUuid(staticMeshUuid)
{
  // There should be no way to create a voxelizer without valid ResourceIndex
  Q_ASSERT(resourceIndex != nullptr);

  if(!resourceIndex->staticMeshAssetsFiles.contains(staticMeshUuid))
    throw GLRT_EXCEPTION(QString("Can't voxelize the not registered static mesh %0").arg(staticMeshUuid.toString()));

  staticMeshFileName = resourceIndex->staticMeshAssetsFiles.value(staticMeshUuid);
  proxyStaticMeshFileName = resourceIndex->staticMeshAssetsFiles.value(proxyStaticMeshUuid.isNull() ? staticMeshUuid : proxyStaticMeshUuid);
  voxelFileName = voxelMetaDataFilenameForMesh(staticMeshFileName);

  shouldRevoxelizeMesh = SHOULD_CONVERT(voxelFileName, proxyStaticMeshFileName);

  if(proxyStaticMeshFileName != staticMeshFileName)
    qDebug() << "Voxelizer: using proxy Mesh " << QFileInfo(proxyStaticMeshFileName).fileName() << "instead of the mesh" << QFileInfo(staticMeshFileName).fileName();
}

Voxelizer::FileNames::FileNames(ResourceIndex* resourceIndex, const QSet<Uuid<StaticMesh>>& staticMeshUuids, const Uuid<StaticMesh>& instanceAnchor)
  : FileNames(resourceIndex, instanceAnchor)
{
  if(staticMeshUuids.isEmpty())
    throw GLRT_EXCEPTION(QString("Can't voxelize the not registered static mesh %0").arg(staticMeshUuid.toString()));

  for(const Uuid<StaticMesh>& staticMeshUuid : staticMeshUuids)
  {
    FileNames f(resourceIndex, staticMeshUuid);

    this->shouldRevoxelizeMesh |= SHOULD_CONVERT(this->voxelFileName, f.staticMeshFileName);
  }
}

void Voxelizer::voxelize(const Uuid<StaticMesh>& staticMeshUuid, MeshType meshType)
{
  voxelize(staticMeshUuid, meshType, staticMeshUuid);
}

void Voxelizer::voxelize(const Uuid<StaticMesh>& staticMeshUuid, MeshType meshType, const Uuid<StaticMesh>& proxyMeshUuid)
{
  CpuVoxelizerImplementation fallbackVoxelizationImplementation;
  Q_UNUSED(fallbackVoxelizationImplementation);

  FileNames fileNames(this->resourceIndex, staticMeshUuid, proxyMeshUuid);

  if(fileNames.shouldRevoxelizeMesh)
    revoxelizeMesh(fileNames, meshType, signedDistanceField);

  registerToIndex(fileNames);
}

void Voxelizer::registerToIndex(const FileNames& fileNames)
{
  VoxelFile voxelFile;
  voxelFile.load(fileNames.voxelFileName, fileNames.staticMeshUuid);

  TextureSampler textureSampler;
  textureSampler.description.maxLod = 0;
  textureSampler.description.minLod = 0;
  textureSampler.description.maxAnisotropy = 1;
  textureSampler.description.magFilter = gl::SamplerObject::Filter::LINEAR;
  textureSampler.description.minFilter = gl::SamplerObject::Filter::LINEAR;
  textureSampler.description.mipFilter = gl::SamplerObject::Filter::LINEAR;
  textureSampler.description.borderHandlingU = gl::SamplerObject::Border::CLAMP;
  textureSampler.description.borderHandlingV = gl::SamplerObject::Border::CLAMP;
  textureSampler.description.borderHandlingW = gl::SamplerObject::Border::CLAMP;

  for(auto i=voxelFile.textureFiles.begin(); i!=voxelFile.textureFiles.end(); ++i)
  {
    Uuid<Texture> textureUuid = Uuid<Texture>::create();
    Uuid<VoxelIndex> voxelUuid = Uuid<VoxelIndex>::create();

    VoxelIndex voxelIndex;
    voxelIndex.factor = i.value().factor;
    voxelIndex.offset = i.value().offset;
    voxelIndex.gridSize = i.value().gridSize;
    voxelIndex.texture3D = textureUuid;
    voxelIndex.boundingSphere = i.value().boundingSphere;
    voxelIndex.localToVoxelSpace = i.value().localToVoxelSpace;
    voxelIndex.voxelizedAsScenery = i.value().flags.testFlag(VoxelFile::MetaData::Flag::Scenery);

    resourceIndex->registerTexture(textureUuid, i.key(), textureSampler);

    resourceIndex->registerVoxelizedMesh(voxelUuid, fileNames.staticMeshUuid, i.value().fieldType, voxelIndex);
  }
}

void Voxelizer::beginJoinedGroup()
{
  staticMeshesToVoxelize_TwoSided.clear();
  staticMeshesToVoxelize_SingleSided.clear();
  staticMeshesToVoxelize_all.clear();
  staticMeshesToVoxelize_Proxies.clear();

  staticMeshesToVoxelize_anchorMesh = Uuid<StaticMesh>();
  staticMeshesToVoxelize_anchorFrame = CoordFrame();
}

void Voxelizer::addToGroup(const Uuid<StaticMesh>& meshUuid, const CoordFrame& frame, bool two_sided, const Uuid<StaticMesh>& proxyMesh)
{
  Q_ASSERT(!meshUuid.isNull());

  QHash<Uuid<StaticMesh>, QSet<CoordFrame>>& map = two_sided ? staticMeshesToVoxelize_TwoSided : staticMeshesToVoxelize_SingleSided;

  if(meshUuid != proxyMesh && !proxyMesh.isNull())
    staticMeshesToVoxelize_Proxies[meshUuid] = proxyMesh;

  map[meshUuid] << frame;
  staticMeshesToVoxelize_all.insert(meshUuid);

  if(staticMeshesToVoxelize_anchorMesh.isNull() || meshUuid < staticMeshesToVoxelize_anchorMesh)
  {
    staticMeshesToVoxelize_anchorMesh = meshUuid;
    staticMeshesToVoxelize_anchorFrame = frame;
  }
}

void Voxelizer::addToGroup(const Uuid<StaticMesh>& meshUuid, const CoordFrame& frame, bool two_sided)
{
  addToGroup(meshUuid, frame, two_sided, Uuid<StaticMesh>());
}

void Voxelizer::voxelizeJoinedGroup(MeshType meshType)
{
  CpuVoxelizerImplementation fallbackVoxelizationImplementation;
  Q_UNUSED(fallbackVoxelizationImplementation);

  const Uuid<StaticMesh> instanceAnchor = staticMeshesToVoxelize_anchorMesh;
  const CoordFrame anchorFrame = staticMeshesToVoxelize_anchorFrame;
  const QSet<Uuid<StaticMesh>>& allStaticMeshes = staticMeshesToVoxelize_all;

  CoordFrame invAnchorFrame = anchorFrame.inverse();

  FileNames fileNames(this->resourceIndex, allStaticMeshes, instanceAnchor);

  size_t rawDataSize = 0;

  if(fileNames.shouldRevoxelizeMesh)
  {
    TriangleArray triangles;

    for(const Uuid<StaticMesh>& staticMeshUuid : staticMeshesToVoxelize_TwoSided.keys())
    {
      const Uuid<StaticMesh> uuid = staticMeshesToVoxelize_Proxies.value(staticMeshUuid, staticMeshUuid);

      FileNames f(resourceIndex, uuid);

      StaticMesh staticMesh;
      staticMesh.loadFromFile(f.staticMeshFileName);
      rawDataSize += staticMesh.rawDataSize()*size_t(staticMeshesToVoxelize_TwoSided.value(uuid).size());
      TriangleArray staticMeshTriangles = staticMesh.getTriangleArray();
      for(CoordFrame frame : staticMeshesToVoxelize_TwoSided.value(uuid))
      {
        TriangleArray v = staticMeshTriangles;
        v.applyTransformation(invAnchorFrame * frame);
        triangles += v;
        if(meshType != MeshType::TWO_SIDED)
        {
          v.invertNormals();
          triangles += v;
        }
      }
    }

    for(const Uuid<StaticMesh>& staticMeshUuid : staticMeshesToVoxelize_SingleSided.keys())
    {
      const Uuid<StaticMesh> uuid = staticMeshesToVoxelize_Proxies.value(staticMeshUuid, staticMeshUuid);

      FileNames f(resourceIndex, uuid);

      StaticMesh staticMesh = StaticMesh::loadFromFile(f.staticMeshFileName);
      rawDataSize += staticMesh.rawDataSize()*size_t(staticMeshesToVoxelize_SingleSided.value(uuid).size());
      TriangleArray staticMeshTriangles = staticMesh.getTriangleArray();
      for(CoordFrame frame : staticMeshesToVoxelize_SingleSided.value(uuid))
      {
        TriangleArray v = staticMeshTriangles;
        v.applyTransformation(invAnchorFrame * frame);
        triangles += v;
      }
    }

    if(triangles.vertices.size() == 0)
      throw GLRT_EXCEPTION(QString("Voxelizer::voxelizeJoinedGroup: No vertices found!"));
    revoxelizeMesh(triangles, rawDataSize, fileNames, meshType, signedDistanceField);
  }

  registerToIndex(fileNames);
}

void Voxelizer::revoxelizeMesh(const FileNames& filenames, MeshType meshType, Hints signedDistanceField)
{
  StaticMeshFile staticMeshFile;
  staticMeshFile.load(filenames.proxyStaticMeshFileName);
  const StaticMesh& staticMesh = staticMeshFile.staticMesh;

  revoxelizeMesh(staticMesh.getTriangleArray(), staticMesh.rawDataSize(), filenames, meshType, signedDistanceField);
}

void Voxelizer::revoxelizeMesh(const TriangleArray& vertices, size_t rawMeshDataSize, const FileNames& filenames, MeshType meshType, Hints signedDistanceField)
{
  SPLASHSCREEN_MESSAGE(QString("Voxelizing <%0>").arg(QFileInfo(filenames.staticMeshFileName).fileName()));

  qInfo() << "Voxelizing" << filenames.staticMeshFileName;

  VoxelFile voxelFile;
  voxelFile.meshUuid = filenames.staticMeshUuid;

  QString signedDistanceFieldFileName = filenames.staticMeshFileName + ".signed-distance-field.texture";

  if(signedDistanceField.enabled)
  {
    VoxelFile::MetaData metaData = voxelizeImplementation(signedDistanceFieldFileName, vertices, rawMeshDataSize, FieldType::SIGNED_DISTANCE_FIELD, meshType, signedDistanceField);
    metaData.flags = this->voxelizing_scene ? VoxelFile::MetaData::Flag::Scenery : VoxelFile::MetaData::Flag::None;

    voxelFile.textureFiles[signedDistanceFieldFileName] = metaData;

  }

  voxelFile.save(filenames.voxelFileName);
}


QString Voxelizer::voxelMetaDataFilenameForMesh(const QString& staticMeshFileName)
{
  Q_ASSERT(!staticMeshFileName.endsWith("/"));
  Q_ASSERT(!staticMeshFileName.endsWith("\\"));
  return staticMeshFileName + ".voxel-metadata";
}



VoxelFile::MetaData initSize(const AABB& meshBoundingBox, int baseSize, const Voxelizer::Hints& hints)
{
  const glm::vec3& meshBoundingBoxMin = meshBoundingBox.minPoint;
  const glm::vec3& meshBoundingBoxMax = meshBoundingBox.maxPoint;

  const glm::vec3 meshSize = meshBoundingBoxMax-meshBoundingBoxMin;

  const float voxelsPerMeter = hints.scaleFactor * baseSize / glm::max(meshSize.x, glm::max(meshSize.y, meshSize.z));
  const float extend = hints.extend;
  int minSize = hints.minSize;
  int maxSize = hints.maxSize;

  Q_ASSERT(!glm::isinf(voxelsPerMeter));

  VoxelFile::MetaData metaData;

  minSize = int(glm::ceilPowerOfTwo<int>(minSize));
  maxSize = int(glm::ceilPowerOfTwo<int>(maxSize));

  Q_ASSERT(minSize > 1);
  Q_ASSERT(maxSize >= minSize);
  Q_ASSERT(glm::all(glm::lessThanEqual(meshBoundingBoxMin, meshBoundingBoxMax)));

  glm::ivec3 voxels = glm::ceil(voxelsPerMeter * meshSize + extend*2.f);
  if(hints.forcePowerOfTwo)
    voxels = glm::ceilPowerOfTwo(voxels);
  if(hints.forceCube)
    voxels = glm::ivec3(glm::max(voxels.x, glm::max(voxels.y, voxels.z))); // this is untested
  voxels = glm::clamp(voxels, glm::ivec3(minSize), glm::ivec3(maxSize));
  metaData.gridSize = glm::ivec3(voxels);

  glm::vec3 scale = (glm::vec3(voxels)-extend*2.f) / meshSize;

  float uniformScaleFactor = INFINITY;
  int outermost_dimension = 0;

  for(int i=0; i<3; ++i)
  {
    if(glm::isnan(scale[i]) || glm::isinf(scale[i]) || scale[i] <=0.f)
      scale[i] = 1.f;
    else if(scale[i] < uniformScaleFactor)
    {
      uniformScaleFactor = scale[i];
      outermost_dimension = i;
    }
  }

  if(glm::isinf(uniformScaleFactor))
  {
    qWarning() << "uniformScaleFactor == inf";
    uniformScaleFactor = 1.f;
  }

  CoordFrame offsetLocalSpace(-(meshBoundingBoxMin+meshBoundingBoxMax)*0.5f);
  CoordFrame scaleWorldToVoxel(glm::vec3(0), glm::quat::IDENTITY, uniformScaleFactor);
  CoordFrame offsetVoxelSpace(glm::vec3(metaData.gridSize)*0.5f);

  metaData.localToVoxelSpace = offsetVoxelSpace * scaleWorldToVoxel * offsetLocalSpace;

#ifdef QT_DEBUG
  {
    float epsilon = 1.e-4f;
    glm::vec3 vec3_extend(extend);

    VoxelData data;
    data.localToVoxelSpace = metaData.localToVoxelSpace;
    data.voxelCount = metaData.gridSize;
    glm::mat4 _localToVoxelSpace = data.worldToVoxelSpaceMatrix4(CoordFrame());

    glm::vec3 p_min = transform_point(_localToVoxelSpace, meshBoundingBoxMin);
    glm::vec3 p_max = transform_point(_localToVoxelSpace, meshBoundingBoxMax);

    Q_ASSERT(glm::abs(p_min - extend)[outermost_dimension] <= epsilon);
    Q_ASSERT(glm::abs(p_max - (glm::vec3(voxels) - vec3_extend))[outermost_dimension] <= epsilon);

    glm::vec3 p[2] = {p_min, p_max};
    for(int i=0; i<2; ++i)
    {
      Q_ASSERT(glm::all(glm::greaterThanEqual(p[i], glm::vec3(extend-epsilon))));
      Q_ASSERT(glm::all(glm::lessThanEqual(p[i], glm::vec3(voxels)-extend+epsilon)));
    }
  }
#else
  Q_UNUSED(outermost_dimension);
#endif


  return metaData;
}

VoxelGridGeometry Voxelizer::calcVoxelSize(const AABB& boundingBox, int baseSize, bool forcePowerOfTwo, bool forceCube, float extend)
{
  Voxelizer::Hints hints;
  hints.minSize = 4;
  hints.maxSize = baseSize;
  hints.extend = extend;
  hints.forcePowerOfTwo = forcePowerOfTwo;
  hints.forceCube = forceCube;

  VoxelFile::MetaData metaData = initSize(boundingBox, baseSize, hints);

  VoxelGridGeometry result;

  result.gridSize = metaData.gridSize;
  result.toVoxelSpace = metaData.localToVoxelSpace;

  return result;
}


VoxelFile::MetaData findBestSize(size_t meshDataSize, size_t bytesPerVoxel, const AABB& meshBoundingBox, const Voxelizer::Hints& hints)
{
  int i;

  Voxelizer::Hints temp_hints = hints;
  temp_hints.scaleFactor = 1.f;

  for(i=hints.minSize; i<=hints.maxSize; ++i)
  {
    VoxelFile::MetaData metaData = initSize(meshBoundingBox, i, temp_hints);

    if(meshDataSize < metaData.rawDataSize(bytesPerVoxel))
    {
      i = glm::max(i-1, hints.minSize);
      break;
    }
  }

  VoxelFile::MetaData metaData = initSize(meshBoundingBox, i, hints);

  qDebug() << "Creating DistanceField " << metaData.gridSize << " with " << metaData.rawDataSize(bytesPerVoxel) << "bytes raw data size for a mesh with " << meshDataSize  << "bytes raw data size using a scale-factor of " << hints.scaleFactor;

  return metaData;
}

VoxelFile::MetaData voxelizeImplementation(const QFileInfo& targetTextureFileName, const TriangleArray& staticMesh, size_t meshDataSize, Voxelizer::FieldType type, Voxelizer::MeshType meshType, const Voxelizer::Hints& hints)
{
  BoundingSphere boundingSphere;
  AABB aabb;

  staticMesh.boundingShapes(boundingSphere, aabb);

  TextureFile textureFile;

  VoxelFile::MetaData metaData;

  switch(type)
  {
  case Voxelizer::FieldType::SIGNED_DISTANCE_FIELD:
  {
    const TextureFile::Type voxelType = GlTexture::Type::FLOAT16;
    const TextureFile::Format voxelFormat = GlTexture::Format::RED;
    const size_t bytesPerVoxel = size_t(GlTexture::bytesPerPixelForFormatType(voxelFormat, voxelType));

    metaData = findBestSize(meshDataSize, bytesPerVoxel, aabb, hints);
    metaData.boundingSphere = boundingSphere;
    metaData.boundingSphere.radius += glm::sqrt(2.0001f) / metaData.localToVoxelSpace.scaleFactor; // avoid culling to discard instances, whould be be still in reach because of interpolation
    metaData.fieldType = type;

    utilities::GlTexture texture = Voxelizer::Implementation::singleton->distanceField(metaData.gridSize, metaData.localToVoxelSpace, staticMesh, meshType);

    Q_ASSERT(TextureFile::Target::TEXTURE_3D == texture.target());

    textureFile.appendImage(texture, voxelType, voxelFormat);
    break;
  }
  }

  textureFile.save(targetTextureFileName);

  return metaData;
}


uint qHash(glrt::scene::resources::Voxelizer::FieldType type)
{
  return ::qHash(uint(type));
}


Voxelizer::Implementation::Implementation()
{
  if(singleton == nullptr)
    singleton = this;
}

Voxelizer::Implementation::~Implementation()
{
  if(singleton == this)
    singleton = nullptr;
}


} // namespace resources
} // namespace scene
} // namespace glrt

