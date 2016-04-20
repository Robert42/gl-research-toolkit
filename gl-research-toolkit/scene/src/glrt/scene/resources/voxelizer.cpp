#include <glrt/scene/resources/voxelizer.h>
#include <glrt/scene/resources/voxel-file.h>
#include <glrt/scene/resources/resource-index.h>
#include <glrt/scene/resources/static-mesh-file.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;

VoxelFile::MetaData voxelizeImplementation(const StaticMesh& staticMesh, const QFileInfo& targetTextureFile, Voxelizer::FieldType type, const Voxelizer::Hints& hints);


Voxelizer::Voxelizer()
{
}

Voxelizer::Voxelizer(ResourceIndex* resourceIndex)
  : resourceIndex(resourceIndex)
{
}

Voxelizer::~Voxelizer()
{
}

bool Voxelizer::enabled() const
{
  return this->resourceIndex != nullptr;
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
  r = angelScriptEngine->RegisterObjectBehaviour(nameVoxelizer, AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_constructor<Voxelizer>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameVoxelizer, "VoxelizerHints signedDistanceField", asOFFSET(Voxelizer,signedDistanceField)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(nameVoxelizer, "ResourceIndex@ resourceIndex", asOFFSET(Voxelizer,resourceIndex)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(nameVoxelizer, "bool get_enabled()", AngelScript::asMETHOD(Voxelizer,enabled), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(nameVoxelizer, "void voxelize(const Uuid<StaticMesh> &in)", AngelScript::asMETHOD(Voxelizer,voxelize), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void Voxelizer::voxelize(const Uuid<StaticMesh>& staticMeshUuid)
{
  if(!enabled())
    throw GLRT_EXCEPTION(QString("Can't voxelize the static mesh %0 with a disabled Voxelizer").arg(staticMeshUuid.toString()));
  if(!resourceIndex->staticMeshAssetsFiles.contains(staticMeshUuid))
    throw GLRT_EXCEPTION(QString("Can't voxelize the not registered static mesh %0").arg(staticMeshUuid.toString()));

  QString staticMeshFileName = resourceIndex->staticMeshAssetsFiles.value(staticMeshUuid);

  StaticMeshFile staticMeshFile;
  staticMeshFile.load(staticMeshFileName);
  const StaticMesh& staticMesh = staticMeshFile.staticMesh;

  VoxelFile voxelFile;
  voxelFile.meshUuid = staticMeshUuid;

  QString voxelFileName = staticMeshFileName + ".voxel-metadata";
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

  Q_ASSERT(maxSize >= minSize);
  Q_ASSERT(glm::all(glm::lessThanEqual(meshBoundingBoxMin, meshBoundingBoxMax)));

  glm::vec3 meshSize = meshBoundingBoxMax-meshBoundingBoxMin;

  glm::ivec3 voxels = glm::ceil(voxelsPerMeter * meshSize + extend*2.f);
  voxels = glm::ceilPowerOfTwo(voxels);
  voxels = glm::clamp(voxels, glm::ivec3(minSize), glm::ivec3(maxSize));
  metaData.gridSize = glm::ivec3(voxels);

  glm::vec3 scale = (glm::vec3(voxels)-extend*2.f) / meshSize;

  float uniformScaleFactor = INFINITY;

  for(int i=0; i<3; ++i)
    if(glm::isnan(scale[i]) || glm::isinf(scale[i]) || scale[i] <=0.f)
      scale[i] = 1.f;
    else
      uniformScaleFactor = glm::min(uniformScaleFactor, uniformScaleFactor);

  if(glm::isinf(uniformScaleFactor))
  {
    qWarning() << "uniformScaleFactor == inf";
    uniformScaleFactor = 1.f;
  }

  CoordFrame offsetLocalSpace(-meshBoundingBoxMin);
  CoordFrame scaleWorldToVoxel(glm::vec3(0), glm::quat(), uniformScaleFactor);
  CoordFrame offsetVoxelSpace(glm::vec3(-extend));

  metaData.localToVoxelSpace = offsetVoxelSpace * scaleWorldToVoxel * offsetLocalSpace;
  return metaData;
}

VoxelFile::MetaData voxelizeImplementation(const StaticMesh& staticMesh, const QFileInfo& targetTextureFile, Voxelizer::FieldType type, const Voxelizer::Hints& hints)
{
  AABB aabb = staticMesh.boundingBox();

  VoxelFile::MetaData metaData = initSize(aabb, hints);
  metaData.fieldType = type;

  // #TODO do the voxelization itself
  qDebug() << "TODO: do the voxelization itself ("<<targetTextureFile.absoluteFilePath() <<")";

  return metaData;
}


} // namespace resources
} // namespace scene
} // namespace glrt
