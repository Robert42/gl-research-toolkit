#include <glrt/scene/resources/voxelizer.h>
#include <glrt/scene/resources/voxel-file.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/static-mesh-file.h>
#include <glrt/toolkit/geometry.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;

VoxelFile::MetaData voxelizeImplementation(const StaticMesh& staticMesh, const QFileInfo& targetTextureFileName, Voxelizer::FieldType type, const Voxelizer::Hints& hints);


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

  r = angelScriptEngine->RegisterObjectType(nameHints, sizeof(Hints), AngelScript::asOBJ_VALUE|AngelScript::asOBJ_POD|AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour(nameHints, AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_constructor<Hints>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameHints, "TextureType voxelType", asOFFSET(Hints,voxelType)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameHints, "float extend", asOFFSET(Hints,extend)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameHints, "int minSize", asOFFSET(Hints,minSize)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameHints, "int maxSize", asOFFSET(Hints,maxSize)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameHints, "float voxelsPerMeter", asOFFSET(Hints,voxelsPerMeter)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType(nameVoxelizer, sizeof(Voxelizer), AngelScript::asOBJ_VALUE|AngelScript::asOBJ_POD|AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour(nameVoxelizer, AngelScript::asBEHAVE_CONSTRUCT, "void f(ResourceIndex@ index)", AngelScript::asFUNCTION((&AngelScriptIntegration::wrap_constructor<Voxelizer, ResourceIndex*>)), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameVoxelizer, "ResourceIndex@ resourceIndex", asOFFSET(Voxelizer,resourceIndex)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameVoxelizer, "VoxelizerHints signedDistanceField", asOFFSET(Voxelizer,signedDistanceField)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(nameVoxelizer, "void voxelize(const Uuid<StaticMesh> &in)", AngelScript::asMETHOD(Voxelizer,voxelize), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void Voxelizer::voxelize(const Uuid<StaticMesh>& staticMeshUuid)
{
  // There should be no way to create a voxelizer without valid ResourceIndex
  Q_ASSERT(resourceIndex != nullptr);

  if(!resourceIndex->staticMeshAssetsFiles.contains(staticMeshUuid))
    throw GLRT_EXCEPTION(QString("Can't voxelize the not registered static mesh %0").arg(staticMeshUuid.toString()));

  QString staticMeshFileName = resourceIndex->staticMeshAssetsFiles.value(staticMeshUuid);
  QString voxelFileName = staticMeshFileName + ".voxel-metadata";

  // #TODO: wenn die voxel metafile bereits existiert die laden und verwendenn anstattalles nochmal neu zu importieren
  bool shouldRevoxelizeMesh = true;
  if(shouldRevoxelizeMesh)
    revoxelizeMesh(staticMeshUuid, staticMeshFileName, voxelFileName);

  VoxelFile voxelFile;
  voxelFile.load(voxelFileName, staticMeshUuid);

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

    TextureSampler textureSampler;
    resourceIndex->registerTexture(textureUuid, i.key(), textureSampler);

    resourceIndex->registerVoxelizedMesh(voxelUuid, staticMeshUuid, i.value().fieldType, voxelIndex);
  }
}

void Voxelizer::revoxelizeMesh(const Uuid<StaticMesh>& staticMeshUuid, const QString& staticMeshFileName, const QString& voxelFileName)
{
  SPLASHSCREEN_MESSAGE(QString("Voxelizing <%0>").arg(QFileInfo(staticMeshFileName).fileName()));

  StaticMeshFile staticMeshFile;
  staticMeshFile.load(staticMeshFileName);
  const StaticMesh& staticMesh = staticMeshFile.staticMesh;

  VoxelFile voxelFile;
  voxelFile.meshUuid = staticMeshUuid;

  QString signedDistanceFieldFileName = staticMeshFileName + ".signed-distance-field.texture";

  if(signedDistanceField.enabled)
    voxelFile.textureFiles[signedDistanceFieldFileName] = voxelizeImplementation(staticMesh, signedDistanceFieldFileName, FieldType::SIGNED_DISTANCE_FIELD, signedDistanceField);

  voxelFile.save(voxelFileName);
}



VoxelFile::MetaData initSize(const AABB& meshBoundingBox, const Voxelizer::Hints& hints)
{
  const glm::vec3& meshBoundingBoxMin = meshBoundingBox.minPoint;
  const glm::vec3& meshBoundingBoxMax = meshBoundingBox.maxPoint;

  float voxelsPerMeter = hints.voxelsPerMeter;
  float extend = hints.extend;
  int minSize = hints.minSize;
  int maxSize = hints.maxSize;

  VoxelFile::MetaData metaData;

  minSize = int(glm::ceilPowerOfTwo<int>(minSize));
  maxSize = int(glm::ceilPowerOfTwo<int>(maxSize));

  Q_ASSERT(minSize > 1);
  Q_ASSERT(maxSize >= minSize);
  Q_ASSERT(glm::all(glm::lessThanEqual(meshBoundingBoxMin, meshBoundingBoxMax)));

  glm::vec3 meshSize = meshBoundingBoxMax-meshBoundingBoxMin;

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
#endif


  return metaData;
}

void voxelizeToSphere(QVector<float>& data, const glm::ivec3& gridSize, const glm::vec3& origin, float radius)
{
#pragma omp parallel for
    for(int x=0; x<gridSize.x; ++x)
      for(int y=0; y<gridSize.y; ++y)
        for(int z=0; z<gridSize.z; ++z)
          data[x + gridSize.x * (y + gridSize.y * z)] = distance(glm::vec3(x,y,z), origin) - radius;
}

void voxelizeToSphere(QVector<float>& data, const glm::ivec3& gridSize)
{
  voxelizeToSphere(data, gridSize, glm::vec3(gridSize)*.5f, glm::min(gridSize.x, glm::min(gridSize.y, gridSize.z))*0.5f);
}

VoxelFile::MetaData voxelizeImplementation(const StaticMesh& staticMesh, const QFileInfo& targetTextureFileName, Voxelizer::FieldType type, const Voxelizer::Hints& hints)
{
  AABB aabb = staticMesh.boundingBox();

  VoxelFile::MetaData metaData = initSize(aabb, hints);
  metaData.fieldType = type;

  TextureFile textureFile;
  QVector<float> data;

  switch(type)
  {
  case Voxelizer::FieldType::SIGNED_DISTANCE_FIELD:
    data.resize(metaData.gridSize.x * metaData.gridSize.y * metaData.gridSize.z);
    voxelizeToSphere(data, metaData.gridSize);


    // #TODO do the voxelization itself
    qDebug() << "TODO: do the voxelization itself ("<<targetTextureFileName.absoluteFilePath() <<")";
    textureFile.appendImage(data,metaData.gridSize, TextureFile::Target::TEXTURE_3D, TextureFile::Type::FLOAT16, TextureFile::Format::RED);
    break;
  }

  textureFile.save(targetTextureFileName);

  return metaData;
}


uint qHash(glrt::scene::resources::Voxelizer::FieldType type)
{
  return ::qHash(uint(type));
}


} // namespace resources
} // namespace scene
} // namespace glrt

