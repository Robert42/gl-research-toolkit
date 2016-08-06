#include <glrt/scene/resources/light-source.h>
#include <glrt/scene/light-component.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;

LightSource::LightSource(const RectAreaLight& rectAreaLight)
  : rect_area_light(rectAreaLight),
    type(Type::RECT_AREA_LIGHT)
{
}

LightSource::LightSource(const SphereAreaLight& sphereAreaLight)
  : sphere_area_light(sphereAreaLight),
    type(Type::SPHERE_AREA_LIGHT)
{
}

inline void as_init_rect_area_light(LightSource::RectAreaLight* rect_area_light)
{
  *rect_area_light = LightSource::RectAreaLight();
}

inline void as_rect_area_light_set_size(LightSource::RectAreaLight* rect_area_light, const glm::vec2* size)
{
  rect_area_light->half_width = size->x*0.5f;
  rect_area_light->half_height = size->y*0.5f;
}

inline glm::vec2 as_rect_area_light_get_size(LightSource::RectAreaLight* rect_area_light)
{
  return glm::vec2(rect_area_light->half_width,
                   rect_area_light->half_height) * 2.0f;
}

inline void as_init_sphere_area_light(LightSource::SphereAreaLight* sphere_area_light)
{
  *sphere_area_light = LightSource::SphereAreaLight();
}

inline LightSource as_convert_to_rect_area_light_source(const LightSource::RectAreaLight* light)
{
  return LightSource(*light);
}

inline LightSource as_convert_to_sphere_area_light_source(const LightSource::SphereAreaLight* light)
{
  return LightSource(*light);
}


template<typename T>
void registerCommonAreaLightProperties(const char* name)
{
  int r;

  r = angelScriptEngine->RegisterObjectProperty(name, "vec3 color", asOFFSET(LightSource::RectAreaLight, areaLightCommon)+asOFFSET(LightSource::AreaLightCommon, color));AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "float luminous_power", asOFFSET(LightSource::RectAreaLight, areaLightCommon)+asOFFSET(LightSource::AreaLightCommon, luminous_power));AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "float influence_radius", asOFFSET(LightSource::RectAreaLight, areaLightCommon)+asOFFSET(LightSource::AreaLightCommon, influence_radius));AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "vec3 origin", asOFFSET(LightSource::RectAreaLight, areaLightCommon)+sizeof(LightSource::AreaLightCommon));AngelScriptCheck(r);
}

void LightSource::registerAngelScriptTypes()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  int r;

  r = angelScriptEngine->RegisterObjectType("AreaLightCommon", sizeof(LightSource::AreaLightCommon), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  registerCommonAreaLightProperties<LightSource::AreaLightCommon>("AreaLightCommon");

  r = angelScriptEngine->RegisterObjectType("RectAreaLightSource", sizeof(LightSource::RectAreaLight), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("RectAreaLightSource", AngelScript::asBEHAVE_CONSTRUCT, "void ctor()", AngelScript::asFUNCTION(as_init_rect_area_light), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("RectAreaLightSource", "vec3 tangent1", asOFFSET(LightSource::RectAreaLight, tangent1)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("RectAreaLightSource", "vec3 tangent2", asOFFSET(LightSource::RectAreaLight, tangent2)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("RectAreaLightSource", "float half_width", asOFFSET(LightSource::RectAreaLight, half_width)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("RectAreaLightSource", "float half_height", asOFFSET(LightSource::RectAreaLight, half_height)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("RectAreaLightSource", "void set_size(const vec2 &in size)", AngelScript::asFUNCTION(as_rect_area_light_set_size), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("RectAreaLightSource", "vec2 get_size()", AngelScript::asFUNCTION(as_rect_area_light_get_size), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  registerCommonAreaLightProperties<LightSource::RectAreaLight>("RectAreaLightSource");

  r = angelScriptEngine->RegisterObjectType("SphereAreaLightSource", sizeof(LightSource::SphereAreaLight), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("SphereAreaLightSource", AngelScript::asBEHAVE_CONSTRUCT, "void ctor()", AngelScript::asFUNCTION(as_init_sphere_area_light), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("SphereAreaLightSource", "float radius", asOFFSET(LightSource::SphereAreaLight, radius)); AngelScriptCheck(r);
  registerCommonAreaLightProperties<LightSource::SphereAreaLight>("SphereAreaLightSource");

  r = angelScriptEngine->RegisterObjectType("LightSource", sizeof(LightSource), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_DAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("RectAreaLightSource", "LightSource opImplConv()", AngelScript::asFUNCTION(as_convert_to_rect_area_light_source), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("SphereAreaLightSource", "LightSource opImplConv()", AngelScript::asFUNCTION(as_convert_to_sphere_area_light_source), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);


  glrt::Uuid<void>::registerCustomizedUuidType("LightSource", false);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace resources
} // namespace scene
} // namespace glrt

