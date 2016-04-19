#include <glrt/scene/resources/voxelizer.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;


Voxelizer::Voxelizer()
{
}

Voxelizer::~Voxelizer()
{
}


void Voxelizer::registerAngelScriptAPI()
{
  int r;
  const char* name = "Voxelizer";

  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterEnum("VoxelFieldType");
  r = angelScriptEngine->RegisterEnumValue("VoxelFieldType", "SIGNED_DISTANCE_FIELD", int(FieldType::SIGNED_DISTANCE_FIELD)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType(name, sizeof(Voxelizer), AngelScript::asOBJ_VALUE|AngelScript::asOBJ_POD); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour(name, AngelScript::asBEHAVE_CONSTRUCT, "void f(ResourceIndex@ index)", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_constructor<Voxelizer>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "ResourceIndex@ resourceIndex", asOFFSET(Voxelizer,resourceIndex)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "VoxelFieldType fieldType", asOFFSET(Voxelizer,fieldType)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureType voxelType", asOFFSET(Voxelizer,voxelType)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "float extend", asOFFSET(Voxelizer,extend)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "int minSize", asOFFSET(Voxelizer,minSize)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "int maxSize", asOFFSET(Voxelizer,maxSize)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "float voxelsPerMeter", asOFFSET(Voxelizer,voxelsPerMeter)); AngelScriptCheck(r);


  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace resources
} // namespace scene
} // namespace glrt
