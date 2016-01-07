#include <glrt/scene/resources/light-source.h>
#include <glrt/scene/light-component.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;


LightComponent* LightSource::createLightComponent(Entity& entity, const Uuid<LightComponent>& uuid, LightComponent::Interactivity interactivity)
{
  switch(type)
  {
  case Type::SPHERE_AREA_LIGHT:
    return new SphereAreaLightComponent(entity, uuid.cast<SphereAreaLightComponent>(), sphere_area_light, interactivity);
  case Type::RECT_AREA_LIGHT:
    return new RectAreaLightComponent(entity, uuid.cast<RectAreaLightComponent>(), rect_area_light, interactivity);
  default:
    Q_UNREACHABLE();
  }
}

inline void as_init_rect_area_light(LightSource::RectAreaLight* rect_area_light, const glm::vec3* size)
{
  rect_area_light->half_width = size->x*0.5f;
  rect_area_light->half_height = size->y*0.5f;
  //#TODO: other attributes
}

void LightSource::registerAngelScriptTypes()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  int r;

  r = angelScriptEngine->RegisterObjectType("LightSource", sizeof(LightSource), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_AK);
  AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType("RectAreaLightSource", sizeof(LightSource::RectAreaLight), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_AK);
  AngelScriptCheck(r);

  //r = angelScriptEngine->RegisterObjectBehaviour("LightSource", AngelScript::asBEHAVE_CONSTRUCT, "void ctor(string &in)", AngelScript::asFUNCTION(init_custom_uuid_type), AngelScript::asCALL_CDECL_OBJFIRST);
  //AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectBehaviour("RectAreaLightSource", AngelScript::asBEHAVE_CONSTRUCT, "void ctor(const vec2 &in)", AngelScript::asFUNCTION(as_init_rect_area_light), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);


  glrt::Uuid<void>::registerCustomizedUuidType("LightSource", false);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace resources
} // namespace scene
} // namespace glrt

