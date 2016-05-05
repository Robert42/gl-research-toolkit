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

using AngelScriptIntegration::AngelScriptCheck;
using utilities::GlTexture;


Voxelizer::Implementation* Voxelizer::Implementation::singleton = nullptr;

VoxelFile::MetaData voxelizeImplementation(const StaticMesh& staticMesh, const QFileInfo& targetTextureFileName, Voxelizer::FieldType type, Voxelizer::MeshType meshType, const Voxelizer::Hints& hints);


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
  r = angelScriptEngine->RegisterEnumValue("VoxelMeshType", "DEFAULT", int(MeshType::DEFAULT)); AngelScriptCheck(r);
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
  r = angelScriptEngine->RegisterObjectMethod(nameVoxelizer, "void voxelize(const Uuid<StaticMesh> &in, VoxelMeshType meshType=VoxelMeshType::DEFAULT)", AngelScript::asMETHOD(Voxelizer,voxelize), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void Voxelizer::voxelize(const Uuid<StaticMesh>& staticMeshUuid, MeshType meshType)
{
  if(Implementation::singleton == nullptr)
    qWarning() << "Using the CPU voxelizer implementation";
  CpuVoxelizerImplementation fallbackVoxelizationImplementation;

  // There should be no way to create a voxelizer without valid ResourceIndex
  Q_ASSERT(resourceIndex != nullptr);

  if(!resourceIndex->staticMeshAssetsFiles.contains(staticMeshUuid))
    throw GLRT_EXCEPTION(QString("Can't voxelize the not registered static mesh %0").arg(staticMeshUuid.toString()));

  QString staticMeshFileName = resourceIndex->staticMeshAssetsFiles.value(staticMeshUuid);
  QString voxelFileName = staticMeshFileName + ".voxel-metadata";

  bool shouldRevoxelizeMesh = SHOULD_CONVERT(voxelFileName, staticMeshFileName);
  if(shouldRevoxelizeMesh)
    revoxelizeMesh(staticMeshUuid, staticMeshFileName, voxelFileName, meshType);

  VoxelFile voxelFile;
  voxelFile.load(voxelFileName, staticMeshUuid);

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
    voxelIndex.localToVoxelSpace = i.value().localToVoxelSpace;

    resourceIndex->registerTexture(textureUuid, i.key(), textureSampler);

    resourceIndex->registerVoxelizedMesh(voxelUuid, staticMeshUuid, i.value().fieldType, voxelIndex);
  }
}

void Voxelizer::revoxelizeMesh(const Uuid<StaticMesh>& staticMeshUuid, const QString& staticMeshFileName, const QString& voxelFileName, MeshType meshType)
{
  SPLASHSCREEN_MESSAGE(QString("Voxelizing <%0>").arg(QFileInfo(staticMeshFileName).fileName()));

  qInfo() << "Voxelizing" << staticMeshFileName;

  StaticMeshFile staticMeshFile;
  staticMeshFile.load(staticMeshFileName);
  const StaticMesh& staticMesh = staticMeshFile.staticMesh;

  VoxelFile voxelFile;
  voxelFile.meshUuid = staticMeshUuid;

  QString signedDistanceFieldFileName = staticMeshFileName + ".signed-distance-field.texture";

  if(signedDistanceField.enabled)
    voxelFile.textureFiles[signedDistanceFieldFileName] = voxelizeImplementation(staticMesh, signedDistanceFieldFileName, FieldType::SIGNED_DISTANCE_FIELD, meshType, signedDistanceField);

  voxelFile.save(voxelFileName);
}



VoxelFile::MetaData initSize(const AABB& meshBoundingBox, int baseSize, const Voxelizer::Hints& hints)
{
  const glm::vec3& meshBoundingBoxMin = meshBoundingBox.minPoint;
  const glm::vec3& meshBoundingBoxMax = meshBoundingBox.maxPoint;

  const glm::vec3 meshSize = meshBoundingBoxMax-meshBoundingBoxMin;

  const float voxelsPerMeter = baseSize / glm::min(meshSize.x, glm::min(meshSize.y, meshSize.z));
  const float extend = hints.extend;
  int minSize = hints.minSize;
  int maxSize = hints.maxSize;

  VoxelFile::MetaData metaData;

  minSize = int(glm::ceilPowerOfTwo<int>(minSize));
  maxSize = int(glm::ceilPowerOfTwo<int>(maxSize));

  Q_ASSERT(minSize > 1);
  Q_ASSERT(maxSize >= minSize);
  Q_ASSERT(glm::all(glm::lessThanEqual(meshBoundingBoxMin, meshBoundingBoxMax)));

  glm::ivec3 voxels = glm::ceil(voxelsPerMeter * meshSize + extend*2.f);
  voxels = glm::ceilPowerOfTwo(voxels);
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
    glm::mat4 _localToVoxelSpace = data.worldToVoxelSpaceMatrix(CoordFrame());

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

VoxelFile::MetaData voxelizeImplementation(const StaticMesh& staticMesh, const QFileInfo& targetTextureFileName, Voxelizer::FieldType type, Voxelizer::MeshType meshType, const Voxelizer::Hints& hints)
{
  const AABB aabb = staticMesh.boundingBox();
  const size_t meshDataSize = staticMesh.rawDataSize();

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

