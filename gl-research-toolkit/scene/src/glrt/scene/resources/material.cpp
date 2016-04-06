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



QString Material::typeToString(Type type)
{
  QStringList usedFlags;

  if(type.testFlag(TypeFlag::TEXTURED))
    usedFlags << "TEXTURED";
  else
    usedFlags << "PLAIN_COLOR";

  if(type.testFlag(TypeFlag::MASKED))
    usedFlags << "MASKED";
  else if(type.testFlag(TypeFlag::TRANSPARENT))
    usedFlags << "TRANSPARENT";
  else
    usedFlags << "OPAQUE";

  if(type.testFlag(TypeFlag::TWO_SIDED))
    usedFlags << "TWO_SIDED";

  return usedFlags.join("|");
}

using AngelScriptIntegration::AngelScriptCheck;

Material::Material(const PlainColor& plainColor, Type type)
  : plainColor(plainColor),
    type(type),
    materialUser(UuidIndex::null_index<0>())
{
  Q_ASSERT(!type.testFlag(TypeFlag::TEXTURED));
}

Material::Material(const Textured<TextureHandle>& textured, Type type)
  : texturesIds(textured),
  type(type),
  materialUser(UuidIndex::null_index<0>())
{
  Q_ASSERT(type.testFlag(TypeFlag::TEXTURED));
}

inline Material as_convert_to_plain_color_material(const Material::PlainColor* plainColor)
{
  return Material(*plainColor);
}

inline Material as_convert_textured_to_material(const Material::Textured<TextureHandle>* textured)
{
  return Material(*textured, static_cast<Material::Type>(textured->type)|Material::TypeFlag::TEXTURED);
}

inline void as_init_plain_color_material(Material::PlainColor* plainColor)
{
  *plainColor = Material::PlainColor();
}

inline void as_init_textured_material(Material::Textured<TextureHandle>* texturedMaterial)
{
  *texturedMaterial = Material::Textured<TextureHandle>();
  texturedMaterial->basecolor_map = TextureManager::instance()->handleFor(uuids::fallbackDiffuseTexture);
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

inline bool as_convert_get_two_sided(Material::Textured<TextureHandle>* texturedMaterial)
{
  return texturedMaterial->type.testFlag(Material::TypeFlag::TWO_SIDED);
}

inline void as_convert_set_two_sided(Material::Textured<TextureHandle>* texturedMaterial, bool value)
{
  if(value)
    texturedMaterial->type |= Material::TypeFlag::TWO_SIDED;
  else
    texturedMaterial->type &= ~Material::Type(Material::TypeFlag::TWO_SIDED);
}

inline bool as_convert_get_masked(Material::Textured<TextureHandle>* texturedMaterial)
{
  return texturedMaterial->type.testFlag(Material::TypeFlag::MASKED);
}

inline void as_convert_set_masked(Material::Textured<TextureHandle>* texturedMaterial, bool value)
{
  if(value)
  {
    texturedMaterial->type &= ~Material::Type(Material::TypeFlag::TRANSPARENT);
    texturedMaterial->type |= Material::TypeFlag::MASKED;
  }else
  {
    texturedMaterial->type &= ~Material::Type(Material::TypeFlag::MASKED);
  }
}

inline bool as_convert_get_transparent(Material::Textured<TextureHandle>* texturedMaterial)
{
  return texturedMaterial->type.testFlag(Material::TypeFlag::TRANSPARENT);
}

inline void as_convert_set_transparent(Material::Textured<TextureHandle>* texturedMaterial, bool value)
{
  if(value)
  {
    texturedMaterial->type &= ~Material::Type(Material::TypeFlag::MASKED);
    texturedMaterial->type |= Material::TypeFlag::TRANSPARENT;
  }else
  {
    texturedMaterial->type &= ~Material::Type(Material::TypeFlag::TRANSPARENT);
  }
}

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

  const char* type = "TexturedMaterial";
  r = angelScriptEngine->RegisterObjectType(type, sizeof(Material::Textured<TextureHandle>), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour(type, AngelScript::asBEHAVE_CONSTRUCT, "void ctor()", AngelScript::asFUNCTION(as_init_textured_material), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(type, "vec4 tint", asOFFSET(Material::Textured<TextureHandle>, tint)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(type, "vec2 srmo_range_0", asOFFSET(Material::Textured<TextureHandle>, srmo_range_0)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(type, "vec2 srmo_range_1", asOFFSET(Material::Textured<TextureHandle>, srmo_range_1)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(type, "float emission_factor", asOFFSET(Material::Textured<TextureHandle>, emission_factor)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(type, "TextureHandle basecolor_map", asOFFSET(Material::Textured<TextureHandle>, basecolor_map)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(type, "TextureHandle normal_map", asOFFSET(Material::Textured<TextureHandle>, normal_map)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(type, "TextureHandle height_map", asOFFSET(Material::Textured<TextureHandle>, height_map)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(type, "TextureHandle srmo_map", asOFFSET(Material::Textured<TextureHandle>, srmo_map)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(type, "TextureHandle emission_map", asOFFSET(Material::Textured<TextureHandle>, emission_map)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(type, "Material opImplConv()", AngelScript::asFUNCTION(as_convert_textured_to_material), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(type, "bool get_two_sided()", AngelScript::asFUNCTION(as_convert_get_two_sided), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(type, "void set_two_sided(bool two_sided)", AngelScript::asFUNCTION(as_convert_set_two_sided), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(type, "bool get_masked()", AngelScript::asFUNCTION(as_convert_get_masked), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(type, "void set_masked(bool masked)", AngelScript::asFUNCTION(as_convert_set_masked), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(type, "bool get_transparent()", AngelScript::asFUNCTION(as_convert_get_transparent), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(type, "void set_transparent(bool transparent)", AngelScript::asFUNCTION(as_convert_set_transparent), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectBehaviour("TextureHandle", AngelScript::asBEHAVE_CONSTRUCT, "void ctor(Uuid<Texture> &in texture, const TextureSampler &in textureSampler)", AngelScript::asFUNCTION(as_init_texture_handle_ts), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

const void* Material::data() const
{
  return &plainColor;
}


bool Material::isTextureType() const
{
  return type.testFlag(TypeFlag::TEXTURED);
}


void Material::prepareForGpuBuffer()
{
  TextureManager& textureManager = *TextureManager::instance();

  if(isTextureType())
  {
    if(textureHandleType == TextureHandleType::Ids)
    {
      textureGpuPtrs.basecolor_map = textureManager.gpuHandle(texturesIds.basecolor_map);
      textureGpuPtrs.normal_map = textureManager.gpuHandle(texturesIds.normal_map);
      textureGpuPtrs.height_map = textureManager.gpuHandle(texturesIds.height_map);
      textureGpuPtrs.srmo_map = textureManager.gpuHandle(texturesIds.srmo_map);
      textureGpuPtrs.emission_map = textureManager.gpuHandle(texturesIds.emission_map);
      textureHandleType = TextureHandleType::GpuPtrs;
    }
  }
}

bool Material::operator<(const Material& other) const
{
  if(this->isTextureType())
    return concatenated_lessThan(this->type, other.type, this->materialUser, other.materialUser, this->textureGpuPtrs.basecolor_map, other.textureGpuPtrs.basecolor_map, this->textureGpuPtrs.normal_map, other.textureGpuPtrs.normal_map);
  else
    return concatenated_lessThan(this->type, other.type, this->materialUser, other.materialUser);
}


int qHash(Material::Type materialType)
{
  return ::qHash(static_cast<int>(materialType));
}


} // namespace resources
} // namespace scene
} // namespace glrt

