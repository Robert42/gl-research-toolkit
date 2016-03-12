#include <GL/glew.h>
#include <glrt/scene/resources/texture-file.h>
#include <glrt/toolkit/plain-old-data-stream.h>
#include <angelscript-integration/collection-converter.h>
#include <angelscript-integration/ref-counted-object.h>

#include <SOIL/SOIL.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;

int TextureFile::ImportSettings::bytesPerPixelForType(Type type)
{
  switch(type)
  {
  case Type::UINT8:
  case Type::INT8:
    return 1;
  case Type::UINT16:
  case Type::INT16:
  case Type::FLOAT16:
    return 2;
  case Type::UINT32:
  case Type::INT32:
  case Type::FLOAT32:
    return 4;
  }
  Q_UNREACHABLE();
}

class TextureFile::ImportedGlTexture
{
public:
  GLuint textureId;

  ImportedGlTexture(GLuint textureId)
    : textureId(textureId)
  {
  }

  ~ImportedGlTexture()
  {
    GL_CALL(glDeleteTextures, 1, &textureId);
  }

  ImportedGlTexture(const ImportedGlTexture&) = delete;
  ImportedGlTexture&operator=(const ImportedGlTexture&) = delete;

  GLint width(int level) const
  {
    Q_ASSERT(level >= 0);
    Q_ASSERT(level < 65536);

    GLint value;
    GL_CALL(glGetTextureLevelParameteriv, textureId, level, GL_TEXTURE_WIDTH, &value);
    Q_ASSERT(value > 0);
    Q_ASSERT(value < 65536);
    return value;
  }

  GLint height(int level) const
  {
    Q_ASSERT(level >= 0);
    Q_ASSERT(level < 65536);

    GLint value;
    GL_CALL(glGetTextureLevelParameteriv, textureId, level, GL_TEXTURE_HEIGHT, &value);
    Q_ASSERT(value > 0);
    Q_ASSERT(value < 65536);
    return value;
  }

  GLint depth(int level) const
  {
    Q_ASSERT(level >= 0);
    Q_ASSERT(level < 65536);

    GLint value;
    GL_CALL(glGetTextureLevelParameteriv, textureId, level, GL_TEXTURE_DEPTH, &value);
    Q_ASSERT(value > 0);
    Q_ASSERT(value < 65536);
    return value;
  }

  QPair<UncompressedImage, QVector<byte>> uncompressed2DImage(int level,
                                                              ImportSettings::Format format,
                                                              ImportSettings::Type type) const
  {
    UncompressedImage image;

    image.width = quint16(this->width(level));
    image.height = quint16(this->height(level));
    image.mipmap = quint16(level);
    image.target = ImportSettings::Target::TEXTURE_2D;
    image.format = format;
    image.type = type;
    image.rawDataLength = image.width * image.height * ImportSettings::bytesPerPixelForType(type);

    QVector<byte> rawData;
    rawData.reserve(image.rawDataLength);

    GL_CALL(glGetTextureImage, textureId, level, GLenum(format), GLenum(type), rawData.length(), rawData.data());

    return qMakePair(image, rawData);
  }
};

TextureFile::TextureFile()
{
}

void TextureFile::import(QFileInfo& srcFile, const ImportSettings& importSettings)
{
  const std::string sourceFilename = srcFile.absoluteFilePath().toStdString();

  // #TODO: use the importSettings for the flags
  const quint32 flags = SOIL_FLAG_POWER_OF_TWO;

  ImportedGlTexture textureInformation(SOIL_load_OGL_texture(sourceFilename.c_str(),
                                                             SOIL_LOAD_AUTO,
                                                             SOIL_CREATE_NEW_ID,
                                                             flags));

  // load the image data
  const int firstMipMap = 0;
  // #TODO
  const int lastMipMap = 0;

  if(importSettings.compression == ImportSettings::Compression::NONE)
  {
    for(int level=firstMipMap; level<=lastMipMap; ++level)
    {
      QPair<UncompressedImage, QVector<byte>> image =  textureInformation.uncompressed2DImage(level, importSettings.format, importSettings.type);

      image.first.rawDataStart = this->appendRawData(image.second);

      this->uncompressedImages.append(image.first);
    }
  }else
  {
    Q_UNREACHABLE();
  }
}

void TextureFile::save(QFileInfo& textureFile)
{
  QFile file(textureFile.absoluteFilePath());

  if(!file.open(QFile::WriteOnly))
    throw GLRT_EXCEPTION(QString("Can't write texture file <%0>").arg(textureFile.absoluteFilePath()));

  Q_ASSERT(uncompressedImages.length() < 0x10000);

  Header header;
  header.numUncompressedImages = quint16(uncompressedImages.length());
  header.numCompressedImages = 0;

  writeValue(file, header);

  for(const UncompressedImage& image : this->uncompressedImages)
    writeValue(file, image);
  for(const CompressedImage& image : this->compressedImages)
    writeValue(file, image);

  for(const QVector<byte>& rawData : this->rawData)
    file.write(reinterpret_cast<const char*>(rawData.data()), rawData.length());
}

quint32 TextureFile::appendRawData(const QVector<byte>& rawData)
{
  quint32 offset = quint32(sizeof(Header));

  offset += quint32(uncompressedImages.length()) * quint32(sizeof(CompressedImage));
  offset += quint32(compressedImages.length()) * quint32(sizeof(UncompressedImage));

  for(const QVector<byte>& rawData : this->rawData)
    offset += quint32(rawData.length());

  this->rawData.insert(offset, rawData);

  return offset;
}

void TextureFile::clear()
{
  compressedImages.clear();
  uncompressedImages.clear();
  rawData.clear();
}

quint64 TextureFile::magicNumber()
{
  struct Str
  {
    char str[8];
  };
  union
  {
    quint64 magicNumber;
    Str str;
  };

  std::memcpy(str.str, "glrt-tex", 8);

  return magicNumber;
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
