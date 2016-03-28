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

Material::Material(const Textured<TextureHandle>& textured, Type type)
  : texturesIds(textured),
  type(type),
  materialUser(UuidIndex::null_index<0>())
{
  Q_ASSERT(type != Type::PLAIN_COLOR);
}

inline Material as_convert_to_plain_color_material(const Material::PlainColor* plainColor)
{
  return Material(*plainColor);
}

inline Material as_convert_textured_opaque_to_material(const Material::Textured<TextureHandle>* textured)
{
  return Material(*textured, Material::Type::TEXTURED_OPAQUE);
}

inline Material as_convert_textured_masked_to_material(const Material::Textured<TextureHandle>* textured)
{
  return Material(*textured, Material::Type::TEXTURED_MASKED);
}

inline Material as_convert_textured_transparent_to_material(const Material::Textured<TextureHandle>* textured)
{
  return Material(*textured, Material::Type::TEXTURED_TRANSPARENT);
}

inline void as_init_plain_color_material(Material::PlainColor* plainColor)
{
  *plainColor = Material::PlainColor();
}

inline void as_init_textured_material(Material::Textured<TextureHandle>* texturedMaterial)
{
  *texturedMaterial = Material::Textured<TextureHandle>();
  texturedMaterial->diffuse_map = TextureManager::instance()->handleFor(uuids::fallbackDiffuseTexture);
  texturedMaterial->normal_map = TextureManager::instance()->handleFor(uuids::fallbackNormalTexture);
  texturedMaterial->emission_map = TextureManager::instance()->handleFor(uuids::blackTexture);
  texturedMaterial->srmo_map = TextureManager::instance()->handleFor(uuids::blackTexture);
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
  as_init_texture_handle_t(textureHandle, scene::resources::uuids::fallbackDiffuseTexture);
}

// #TODO also sort materials by the used textures

void Material::registerAngelScriptTypes()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  int r;
  static_assert(sizeof(TextureHandle) == sizeof(GLuint64), "wrong size");
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

  for(const char* type : {"TexturedMaterialOpaque", "TexturedMaterialMasked", "TexturedMaterialTransparent"})
  {
    r = angelScriptEngine->RegisterObjectType(type, sizeof(Material::Textured<TextureHandle>), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
    r = angelScriptEngine->RegisterObjectBehaviour(type, AngelScript::asBEHAVE_CONSTRUCT, "void ctor()", AngelScript::asFUNCTION(as_init_textured_material), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
    r = angelScriptEngine->RegisterObjectProperty(type, "vec4 tint", asOFFSET(Material::Textured<TextureHandle>, tint)); AngelScriptCheck(r);
    r = angelScriptEngine->RegisterObjectProperty(type, "vec2 smoothness_range", asOFFSET(Material::Textured<TextureHandle>, smoothness_range)); AngelScriptCheck(r);
    r = angelScriptEngine->RegisterObjectProperty(type, "vec2 occlusion_range", asOFFSET(Material::Textured<TextureHandle>, occlusion_range)); AngelScriptCheck(r);
    r = angelScriptEngine->RegisterObjectProperty(type, "vec2 reflectance_range", asOFFSET(Material::Textured<TextureHandle>, reflectance_range)); AngelScriptCheck(r);
    r = angelScriptEngine->RegisterObjectProperty(type, "float emission_factor", asOFFSET(Material::Textured<TextureHandle>, emission_factor)); AngelScriptCheck(r);
    r = angelScriptEngine->RegisterObjectProperty(type, "TextureHandle diffuse_map", asOFFSET(Material::Textured<TextureHandle>, diffuse_map)); AngelScriptCheck(r);
    r = angelScriptEngine->RegisterObjectProperty(type, "TextureHandle normal_map", asOFFSET(Material::Textured<TextureHandle>, normal_map)); AngelScriptCheck(r);
    r = angelScriptEngine->RegisterObjectProperty(type, "TextureHandle height_map", asOFFSET(Material::Textured<TextureHandle>, height_map)); AngelScriptCheck(r);
    r = angelScriptEngine->RegisterObjectProperty(type, "TextureHandle srmo_map", asOFFSET(Material::Textured<TextureHandle>, srmo_map)); AngelScriptCheck(r);
    r = angelScriptEngine->RegisterObjectProperty(type, "TextureHandle emission_map", asOFFSET(Material::Textured<TextureHandle>, emission_map)); AngelScriptCheck(r);
  }
  r = angelScriptEngine->RegisterObjectMethod("TexturedMaterialOpaque", "Material opImplConv()", AngelScript::asFUNCTION(as_convert_textured_opaque_to_material), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("TexturedMaterialMasked", "Material opImplConv()", AngelScript::asFUNCTION(as_convert_textured_masked_to_material), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("TexturedMaterialTransparent", "Material opImplConv()", AngelScript::asFUNCTION(as_convert_textured_transparent_to_material), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);


  r = angelScriptEngine->RegisterObjectBehaviour("TextureHandle", AngelScript::asBEHAVE_CONSTRUCT, "void ctor(Uuid<Texture> &in texture, const TextureSampler &in textureSampler)", AngelScript::asFUNCTION(as_init_texture_handle_ts), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

const void* Material::data() const
{
  return &plainColor;
}


void Material::prepareForGpuBuffer()
{
  TextureManager& textureManager = *TextureManager::instance();

  switch(type)
  {
  case Type::TEXTURED_MASKED:
  case Type::TEXTURED_OPAQUE:
  case Type::TEXTURED_TRANSPARENT:
    if(textureHandleType == TextureHandleType::Ids)
    {
      textureGpuPtrs.diffuse_map = textureManager.gpuHandle(texturesIds.diffuse_map);
      textureGpuPtrs.normal_map = textureManager.gpuHandle(texturesIds.normal_map);
      textureGpuPtrs.height_map = textureManager.gpuHandle(texturesIds.height_map);
      textureGpuPtrs.srmo_map = textureManager.gpuHandle(texturesIds.srmo_map);
      textureGpuPtrs.emission_map = textureManager.gpuHandle(texturesIds.emission_map);
      textureHandleType = TextureHandleType::GpuPtrs;
    }
    break;
  case Type::PLAIN_COLOR:
    break;
  }
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

