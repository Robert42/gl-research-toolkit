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

bool Voxelizer::isEnabled() const
{
  return this->resourceIndex != nullptr;
}

void Voxelizer::registerAngelScriptAPI()
{
  int r;
  const char* name = "Voxelizer";

  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterEnum("VoxelFieldType");
  r = angelScriptEngine->RegisterEnumValue("VoxelFieldType", "SIGNED_DISTANCE_FIELD", int(FieldType::SIGNED_DISTANCE_FIELD)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType(name, sizeof(Voxelizer), AngelScript::asOBJ_VALUE|AngelScript::asOBJ_POD|AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour(name, AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_constructor<Voxelizer>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "ResourceIndex@ resourceIndex", asOFFSET(Voxelizer,resourceIndex)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "VoxelFieldType fieldType", asOFFSET(Voxelizer,fieldType)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureType voxelType", asOFFSET(Voxelizer,voxelType)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "float extend", asOFFSET(Voxelizer,extend)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "int minSize", asOFFSET(Voxelizer,minSize)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "int maxSize", asOFFSET(Voxelizer,maxSize)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "float voxelsPerMeter", asOFFSET(Voxelizer,voxelsPerMeter)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "bool get_isEnabled()", AngelScript::asMETHOD(Voxelizer,isEnabled), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "void voxelize(const Uuid<StaticMesh> &in)", AngelScript::asMETHOD(Voxelizer,voxelize), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void Voxelizer::voxelize(const Uuid<StaticMesh>& staticMesh)
{
  if(!isEnabled())
    throw GLRT_EXCEPTION(QString("Can't voxelize the static mesh %0 with a disabled Voxelizer").arg(staticMesh.toString()));
  if(!resourceIndex->staticMeshAssetsFiles.contains(staticMesh))
    throw GLRT_EXCEPTION(QString("Can't voxelize the not registered static mesh %0").arg(staticMesh.toString()));

  QString staticMeshFile = resourceIndex->staticMeshAssetsFiles.value(staticMesh);
  qDebug() << "TODO: voxelize " << staticMeshFile;
}


} // namespace resources
} // namespace scene
} // namespace glrt
