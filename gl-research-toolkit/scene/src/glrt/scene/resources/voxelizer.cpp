#include <glrt/scene/resources/voxelizer.h>
#include <glrt/scene/resources/resource-index.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;


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

void Voxelizer::voxelize(const Uuid<StaticMesh>& staticMesh)
{
  if(!enabled())
    throw GLRT_EXCEPTION(QString("Can't voxelize the static mesh %0 with a disabled Voxelizer").arg(staticMesh.toString()));
  if(!resourceIndex->staticMeshAssetsFiles.contains(staticMesh))
    throw GLRT_EXCEPTION(QString("Can't voxelize the not registered static mesh %0").arg(staticMesh.toString()));

  QString staticMeshFile = resourceIndex->staticMeshAssetsFiles.value(staticMesh);
  qDebug() << "TODO: voxelize " << staticMeshFile;
}


} // namespace resources
} // namespace scene
} // namespace glrt
