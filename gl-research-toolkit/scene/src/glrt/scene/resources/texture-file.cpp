#include <glrt/scene/resources/texture-file.h>
#include <angelscript-integration/collection-converter.h>
#include <angelscript-integration/ref-counted-object.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;

TextureFile::TextureFile()
{
}

void TextureFile::import(QFileInfo& srcFile, const ImportSettings& importSettings)
{
}

void TextureFile::save(QFileInfo& textureFile)
{
}

void TextureFile::ImportSettings::registerType()
{
  int r;
  const char* name = "TextureImportSettings";

  r = angelScriptEngine->RegisterEnum("TextureFormat"); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureFormat", "R", int(Format::RED)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureFormat", "G", int(Format::GREEN)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureFormat", "B", int(Format::BLUE)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureFormat", "RG", int(Format::RG)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureFormat", "RGB", int(Format::RGB)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureFormat", "BGR", int(Format::BGR)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureFormat", "RGBA", int(Format::RGBA)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureFormat", "BGRA", int(Format::BGRA)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterEnum("TextureType"); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "INT8", int(Type::INT8)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "UINT8", int(Type::UINT8)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "INT16", int(Type::INT16)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "UINT16", int(Type::UINT16)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "INT32", int(Type::INT32)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "UINT32", int(Type::UINT32)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "FLOAT16", int(Type::FLOAT16)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "FLOAT32", int(Type::FLOAT32)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType(name, sizeof(ImportSettings), AngelScript::asOBJ_VALUE|AngelScript::asOBJ_POD); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("TextureImportSettings", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_constructor<ImportSettings>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureType type", asOFFSET(ImportSettings,type)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureFormat format", asOFFSET(ImportSettings,format)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "int width", asOFFSET(ImportSettings,width)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "int height", asOFFSET(ImportSettings,height)); AngelScriptCheck(r);
}


} // namespace resources
} // namespace scene
} // namespace glrt
