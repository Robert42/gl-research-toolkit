#include <glrt/scene/resources/texture-sampler.h>
#include <angelscript-integration/collection-converter.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;


TextureSampler::TextureSampler()
  : description(gl::SamplerObject::Filter::LINEAR, gl::SamplerObject::Filter::LINEAR, gl::SamplerObject::Filter::LINEAR, gl::SamplerObject::Border::REPEAT)
{
}

bool TextureSampler::operator==(const TextureSampler& other) const
{
  return other.description == this->description;
}

void TextureSampler::registerType()
{
  Q_ASSERT(asOFFSET(TextureSampler, description) == 0);

  int r;
  const char* name = "TextureSampler";

  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterEnum("TextureFilter");
  r = angelScriptEngine->RegisterEnumValue("TextureFilter", "NEAREST", int(gl::SamplerObject::Filter::NEAREST)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureFilter", "LINEAR", int(gl::SamplerObject::Filter::LINEAR)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterEnum("TextureBorder");
  r = angelScriptEngine->RegisterEnumValue("TextureBorder", "REPEAT", int(gl::SamplerObject::Border::REPEAT)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureBorder", "MIRROR", int(gl::SamplerObject::Border::MIRROR)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureBorder", "CLAMP", int(gl::SamplerObject::Border::CLAMP)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureBorder", "BORDER", int(gl::SamplerObject::Border::BORDER)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterEnum("TextureCompareMode");
  r = angelScriptEngine->RegisterEnumValue("TextureCompareMode", "NONE", int(gl::SamplerObject::CompareMode::NONE)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureCompareMode", "LESS_EQUAL", int(gl::SamplerObject::CompareMode::LESS_EQUAL)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureCompareMode", "GREATER_EQUAL", int(gl::SamplerObject::CompareMode::GREATER_EQUAL)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureCompareMode", "LESS", int(gl::SamplerObject::CompareMode::LESS)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureCompareMode", "GREATER", int(gl::SamplerObject::CompareMode::GREATER)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureCompareMode", "EQUAL", int(gl::SamplerObject::CompareMode::EQUAL)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureCompareMode", "NOTEQUAL", int(gl::SamplerObject::CompareMode::NOTEQUAL)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureCompareMode", "ALWAYS", int(gl::SamplerObject::CompareMode::ALWAYS)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureCompareMode", "NEVER", int(gl::SamplerObject::CompareMode::NEVER)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType(name, sizeof(TextureSampler), AngelScript::asOBJ_VALUE|AngelScript::asOBJ_POD); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour(name, AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_constructor<TextureSampler>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureFilter minFilter", asOFFSET(gl::SamplerObject::Desc,minFilter)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureFilter magFilter", asOFFSET(gl::SamplerObject::Desc,magFilter)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureFilter mipFilter", asOFFSET(gl::SamplerObject::Desc,mipFilter)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureBorder borderHandlingU", asOFFSET(gl::SamplerObject::Desc,borderHandlingU)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureBorder borderHandlingV", asOFFSET(gl::SamplerObject::Desc,borderHandlingV)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureBorder borderHandlingW", asOFFSET(gl::SamplerObject::Desc,borderHandlingW)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "uint maxAnisotropy", asOFFSET(gl::SamplerObject::Desc,maxAnisotropy)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "vec4 borderColor", asOFFSET(gl::SamplerObject::Desc,borderColor)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureCompareMode compareMode", asOFFSET(gl::SamplerObject::Desc,compareMode)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "float minLod", asOFFSET(gl::SamplerObject::Desc,minLod)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "float maxLod", asOFFSET(gl::SamplerObject::Desc,maxLod)); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace resources
} // namespace scene
} // namespace glrt
