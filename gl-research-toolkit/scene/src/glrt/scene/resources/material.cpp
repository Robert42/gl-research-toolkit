#include <glrt/scene/resources/material.h>
#include <glrt/toolkit/uuid.h>
#include <glrt/toolkit/concatenated-less-than.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;

Material::Material(const PlainColor& plainColor)
  : plainColor(plainColor),
    type(Type::PLAIN_COLOR),
    materialUser(UuidIndex::null_index<0>())
{
}

inline Material as_convert_to_plain_color_material(const Material::PlainColor* plainColor)
{
  return Material(*plainColor);
}

inline void as_init_plain_color_material(Material::PlainColor* plainColor)
{
  *plainColor = Material::PlainColor();
}

void Material::registerAngelScriptTypes()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  int r;

  r = angelScriptEngine->RegisterObjectType("PlainColorMaterial", sizeof(Material::PlainColor), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("PlainColorMaterial", AngelScript::asBEHAVE_CONSTRUCT, "void ctor()", AngelScript::asFUNCTION(as_init_plain_color_material), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("PlainColorMaterial", "vec3 base_color", asOFFSET(PlainColor, base_color)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("PlainColorMaterial", "float smoothness", asOFFSET(PlainColor, smoothness)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("PlainColorMaterial", "vec3 emission", asOFFSET(PlainColor, emission)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("PlainColorMaterial", "float metal_mask", asOFFSET(PlainColor, metal_mask)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType("Material", sizeof(Material), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_DAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("PlainColorMaterial", "Material opImplConv()", AngelScript::asFUNCTION(as_convert_to_plain_color_material), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  glrt::Uuid<void>::registerCustomizedUuidType("Material", false);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

const void* Material::data() const
{
  return &plainColor;
}

bool Material::operator<(const Material& other) const
{
  return concatenated_lessThan(this->type, other.type, this->materialUser, this->materialUser);
}


int qHash(Material::Type materialType)
{
  return ::qHash(static_cast<int>(materialType));
}


} // namespace resources
} // namespace scene
} // namespace glrt

