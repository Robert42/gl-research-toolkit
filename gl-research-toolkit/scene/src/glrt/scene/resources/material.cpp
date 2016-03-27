#include <glrt/scene/resources/material.h>
#include <glrt/toolkit/uuid.h>
#include <glrt/toolkit/concatenated-less-than.h>
#include <glrt/scene/resources/texture.h>
#include <glrt/scene/resources/texture-manager.h>
#include <glrt/scene/resources/texture-sampler.h>
#include <glrt/scene/resources/resource-index.h>

namespace glrt {
namespace scene {
namespace resources {

typedef Material::TextureHandle TextureHandle;


QVector<Material::Type> Material::allTypes()
{
  return {Type::PLAIN_COLOR,
        Type::TEXTURED_OPAQUE,
        Type::TEXTURED_MASKED,
        Type::TEXTURED_TRANSPARENT};
}

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

inline void as_init_texture_handle_ts(TextureHandle* textureHandle, const Uuid<Texture>& texture, const TextureSampler& textureSampler)
{
  *textureHandle = TextureManager::instance()->handleFor(texture, textureSampler);
}

inline void as_init_texture_handle_t(TextureHandle* textureHandle, const Uuid<Texture>& texture)
{
  *textureHandle = TextureManager::instance()->handleFor(texture);
}

inline void as_init_texture_handle(TextureHandle* textureHandle)
{
  as_init_texture_handle_t(textureHandle, scene::resources::uuids::fallbackTexture);
}

// #TODO also sort materials by the used textures

void Material::registerAngelScriptTypes()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  int r;
  r = angelScriptEngine->RegisterObjectType("TextureHandle", sizeof(TextureHandle), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("TextureHandle", AngelScript::asBEHAVE_CONSTRUCT, "void ctor()", AngelScript::asFUNCTION(as_init_texture_handle), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("TextureHandle", AngelScript::asBEHAVE_CONSTRUCT, "void ctor(Uuid<Texture> &in texture)", AngelScript::asFUNCTION(as_init_texture_handle_t), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType("PlainColorMaterial", sizeof(Material::PlainColor), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("PlainColorMaterial", AngelScript::asBEHAVE_CONSTRUCT, "void ctor()", AngelScript::asFUNCTION(as_init_plain_color_material), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("PlainColorMaterial", "vec3 base_color", asOFFSET(PlainColor, base_color)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("PlainColorMaterial", "float smoothness", asOFFSET(PlainColor, smoothness)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("PlainColorMaterial", "vec3 emission", asOFFSET(PlainColor, emission)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty("PlainColorMaterial", "float metal_mask", asOFFSET(PlainColor, metal_mask)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType("Material", sizeof(Material), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_DAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("PlainColorMaterial", "Material opImplConv()", AngelScript::asFUNCTION(as_convert_to_plain_color_material), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  glrt::Uuid<void>::registerCustomizedUuidType("Material", false);

  r = angelScriptEngine->RegisterObjectBehaviour("TextureHandle", AngelScript::asBEHAVE_CONSTRUCT, "void ctor(Uuid<Texture> &in texture, const TextureSampler &in textureSampler)", AngelScript::asFUNCTION(as_init_texture_handle_ts), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

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

