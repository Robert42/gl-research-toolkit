#include <QMetaEnum>
#include <GL/glew.h>
#include <glm/gtc/round.hpp>
#include <glrt/scene/resources/texture-file.h>
#include <glrt/toolkit/plain-old-data-stream.h>
#include <angelscript-integration/collection-converter.h>
#include <angelscript-integration/ref-counted-object.h>

#include <SOIL/SOIL.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;

int TextureFile::ImportSettings::channelsPerPixelForFormat(Format format)
{
  switch(format)
  {
  case Format::RED:
    return 1;
  case Format::RG:
    return 2;
  case Format::RGB:
    return 3;
  case Format::RGBA:
    return 4;
  }
  Q_UNREACHABLE();
}

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
  case Type::FLOAT32:
    return 4;
  }
  Q_UNREACHABLE();
}

int TextureFile::ImportSettings::bytesPerPixelForFormatType(Format format, Type type)
{
  return channelsPerPixelForFormat(format) * bytesPerPixelForType(type);
}

GLenum TextureFile::ImportSettings::internalFormat(Format format, Type type, bool* supported)
{
  bool dummy=false;

  if(!supported)
    supported = &dummy;

  *supported = true;

  switch(type)
  {
  case Type::UINT8:
    switch(format)
    {
    case Format::RED:
      return GL_R8;
    case Format::RG:
      return GL_RG8;
    case Format::RGB:
      return GL_RGB8;
    case Format::RGBA:
      return GL_RGBA8;
    }
    break;
  case Type::INT8:
    switch(format)
    {
    case Format::RED:
      return GL_R8_SNORM;
    case Format::RG:
      return GL_RG8_SNORM;
    case Format::RGB:
      return GL_RGB8_SNORM;
    case Format::RGBA:
      return GL_RGBA8_SNORM;
    }
    break;
  case Type::UINT16:
    switch(format)
    {
    case Format::RED:
      return GL_R16;
    case Format::RG:
      return GL_RG16;
    case Format::RGB:
      break;
    case Format::RGBA:
      return GL_RGBA16;
    }
    break;
  case Type::INT16:
    switch(format)
    {
    case Format::RED:
      return GL_R16_SNORM;
    case Format::RG:
      return GL_RG16_SNORM;
    case Format::RGB:
      return GL_RGB16_SNORM;
    case Format::RGBA:
      break;
    }
    break;
  case Type::FLOAT16:
    switch(format)
    {
    case Format::RED:
      return GL_R16F;
    case Format::RG:
      return GL_RG16F;
    case Format::RGB:
      return GL_RGB16F;
    case Format::RGBA:
      return GL_RGBA16F;
    }
    break;
  case Type::FLOAT32:
    switch(format)
    {
    case Format::RED:
      return GL_R32F;
    case Format::RG:
      return GL_RG32F;
    case Format::RGB:
      return GL_RGB32F;
    case Format::RGBA:
      return GL_RGBA32F;
    }
    break;
  }
  Q_UNREACHABLE();
  *supported = false;
  return GL_R8;
}

class TextureFile::ImportedGlTexture
{
public:
  GLuint textureId;

  ImportedGlTexture()
  {
    GL_CALL(glGenTextures, 1, &textureId);
  }

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
                                                              TextureFile::Format format,
                                                              TextureFile::Type type) const
  {
    UncompressedImage image;

    image.type = type;
    image.format = format;
    // format and type must be set before calling calcRowStride!

    image.width = quint32(this->width(level));
    image.rowStride = image.calcRowStride();
    image.height = quint32(this->height(level));
    image.depth = 1;
    image.mipmap = quint32(level);
    image.target = TextureFile::Target::TEXTURE_2D;
    image.rawDataLength = image.rowStride * image.height;

    bool supportedFormat;
    ImportSettings::internalFormat(image.format, image.type, &supportedFormat);
    Q_ASSERT(supportedFormat);

    QVector<byte> rawData;
    rawData.resize(int(image.rawDataLength));

    GL_CALL(glGetTextureImage, textureId, level, GLenum(format), GLenum(type), rawData.length(), rawData.data());

    return qMakePair(image, rawData);
  }

  void setUncompressed2DImage(const UncompressedImage& image, const void* data)
  {
    Q_ASSERT(image.width != 0);
    Q_ASSERT(image.height != 0);
    Q_ASSERT(image.depth != 0);
    Q_ASSERT(image.rowStride == image.calcRowStride());
    Q_ASSERT(image.rawDataLength == image.rowStride*image.height*image.depth);
    Q_ASSERT(image.rawDataLength <= std::numeric_limits<int>::max());

    bool supportedFormat;
    GLenum internalFormat = ImportSettings::internalFormat(image.format, image.type, &supportedFormat);
    Q_ASSERT(supportedFormat);

    GL_CALL(glBindTexture, static_cast<GLenum>(image.target), this->textureId);

    switch(image.target)
    {
    case Target::TEXTURE_1D:
      Q_ASSERT(image.width > 0 && image.depth == 1 && image.height==1);
      GL_CALL(glTexImage1D, static_cast<GLenum>(image.target), image.mipmap, internalFormat, image.width, 0, static_cast<GLenum>(image.format), static_cast<GLenum>(image.type),data);
      break;
    case Target::CUBE_MAP_POSITIVE_X:
    case Target::CUBE_MAP_NEGATIVE_X:
    case Target::CUBE_MAP_POSITIVE_Y:
    case Target::CUBE_MAP_NEGATIVE_Y:
    case Target::CUBE_MAP_POSITIVE_Z:
    case Target::CUBE_MAP_NEGATIVE_Z:
    case Target::TEXTURE_2D:
      Q_ASSERT(image.width > 0 || image.height > 0 || image.depth == 1);
      GL_CALL(glTexImage2D, static_cast<GLenum>(image.target), image.mipmap, internalFormat, image.width, image.height, 0, static_cast<GLenum>(image.format), static_cast<GLenum>(image.type),data);
      break;
    case Target::TEXTURE_3D:
      Q_ASSERT(image.width > 0 && image.depth > 0 && image.height>0);
      GL_CALL(glTexImage3D, static_cast<GLenum>(image.target), image.mipmap, internalFormat, image.width, image.height, image.depth, 0, static_cast<GLenum>(image.format), static_cast<GLenum>(image.type),data);
      break;
    }

    GL_CALL(glBindTexture, static_cast<GLenum>(image.target), 0);
  }

  void convertToSignedNormalMap(int level)
  {
    QPair<UncompressedImage, QVector<byte>> importedTexture = uncompressed2DImage(level, Format::RGBA, Type::FLOAT32);

    QVector<byte> textureData = importedTexture.second;
    const UncompressedImage& image = importedTexture.first;
    byte* data = reinterpret_cast<byte*>(textureData.data());

    quint32 w = image.width*4;
    quint32 h = image.height * image.depth;
    for(quint32 y=0; y<h; ++y)
    {
      float* line = reinterpret_cast<float*>(data + y * image.rowStride);
      for(quint32 x=0; x<w; ++x)
        line[x] = line[x]*2.f - 1.f;
    }

    setUncompressed2DImage(image, data);
  }
};

TextureFile::TextureFile()
{
}

void TextureFile::import(const QFileInfo& srcFile, const ImportSettings& importSettings)
{
  const std::string sourceFilename = srcFile.absoluteFilePath().toStdString();

  // #TODO: use the importSettings for the flags
  const quint32 flags = SOIL_FLAG_POWER_OF_TWO|SOIL_FLAG_INVERT_Y;

  ImportedGlTexture textureInformation(SOIL_load_OGL_texture(sourceFilename.c_str(),
                                                             SOIL_LOAD_AUTO,
                                                             SOIL_CREATE_NEW_ID,
                                                             flags));

  // load the image data
  const int firstMipMap = 0;
  // #TODO decide, whether to use mipmapping by
  const int lastMipMap = 0;

  if(importSettings.compression == TextureFile::Compression::NONE)
  {
    for(int level=firstMipMap; level<=lastMipMap; ++level)
    {
      if(importSettings.sourceIsNormalMap)
        textureInformation.convertToSignedNormalMap(level);

      QPair<UncompressedImage, QVector<byte>> image =  textureInformation.uncompressed2DImage(level, importSettings.format, importSettings.type);

      image.first.rawDataStart = this->appendRawData(image.second);

      this->uncompressedImages.append(image.first);
    }
  }else
  {
    Q_UNREACHABLE();
  }
}

void TextureFile::save(const QFileInfo& textureFile)
{
  Q_ASSERT(uncompressedImages.length() < 0x10000);

  QByteArray byteArray;
  byteArray.reserve(expectedFileSize());

  quint64 rawDataOffset = sizeof(Header) + quint64(this->uncompressedImages.length())*sizeof(UncompressedImage) + quint64(this->compressedImages.length())*sizeof(CompressedImage);

  Header header;
  header.numUncompressedImages = quint16(uncompressedImages.length());
  header.numCompressedImages = 0;
  header._padding.clear();

  writeValue(byteArray, header);

  for(UncompressedImage image : this->uncompressedImages)
  {
    image.rawDataStart += rawDataOffset;
    writeValue(byteArray, image);
  }
  for(CompressedImage image : this->compressedImages)
  {
    image.rawDataStart += rawDataOffset;
    writeValue(byteArray, image);
  }

  for(const QVector<byte>& rawData : this->rawData)
  {
    const char* data = reinterpret_cast<const char*>(rawData.data());
    int length = rawData.length();

    byteArray.append(data, length);
  }

  QFile file(textureFile.absoluteFilePath());

  if(!file.open(QFile::WriteOnly))
    throw GLRT_EXCEPTION(QString("Can't write texture file <%0>").arg(textureFile.absoluteFilePath()));

  file.write(byteArray);
}

int TextureFile::expectedFileSize() const
{
  int expectedSize = sizeof(Header);

  expectedSize += int(sizeof(UncompressedImage)) * this->uncompressedImages.length();
  expectedSize += int(sizeof(CompressedImage)) * this->compressedImages.length();

  for(const QVector<byte>& rawData : this->rawData)
    expectedSize += rawData.length();
  return expectedSize;
}

GLuint TextureFile::loadFromFile(const QFileInfo& textureFile)
{
  QFile file(textureFile.absoluteFilePath());

  if(!file.open(QFile::ReadOnly))
    throw GLRT_EXCEPTION(QString("Can't read texture file <%0>").arg(textureFile.absoluteFilePath()));


  ImportedGlTexture importedTexture;

  quint64 offsetCheck = 0;

  Header header = readValue<Header>(file);

  if(header.magicNumber != TextureFile::magicNumber())
    throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): Wrong magic number %1.").arg(textureFile.fileName()).arg(header.magicNumber));
  if(header.headerLength != sizeof(Header))
    throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): Unexpected Header size %1.").arg(textureFile.fileName()).arg(header.headerLength));
  if(header.uncompressedImageSize != sizeof(UncompressedImage))
    throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): Unexpected UncompressedImage %1.").arg(textureFile.fileName()).arg(header.uncompressedImageSize));
  if(header.compressedImageSize != 0)
    throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): Unexpected compressedImageSize %1.").arg(textureFile.fileName()).arg(header.compressedImageSize));
  if(header.numCompressedImages != 0)
    throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): Unexpected numCompressedImages %1.").arg(textureFile.fileName()).arg(header.compressedImageSize));

  if(header.numCompressedImages != 0 && header.compressedImageSize != 0)
    throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): Mixing uncompressed and uncompressed subimages within the same file is not possible.").arg(textureFile.fileName()));

  offsetCheck += sizeof(Header);

  QVector<UncompressedImage> uncompressedImages;
  QVector<CompressedImage> compressedImages;

  for(int i=0; i<header.numUncompressedImages; ++i)
  {
    UncompressedImage image = readValue<UncompressedImage>(file);
    offsetCheck += sizeof(UncompressedImage);
    uncompressedImages.append(image);
  }

  for(int i=0; i<header.numCompressedImages; ++i)
  {
    CompressedImage image = readValue<CompressedImage>(file);
    offsetCheck += sizeof(CompressedImage);
    compressedImages.append(image);
  }

  Array<byte> tempBuffer;

  QMap<GLenum, GLint> maxMipMap;
  QMap<GLenum, GLint> minMipMap;

  for(const UncompressedImage& image : uncompressedImages)
  {
    if(offsetCheck != image.rawDataStart)
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): imageDataOffset has the wrong value %1, expected %2").arg(textureFile.fileName()).arg(image.rawDataStart).arg(offsetCheck));

    QMetaEnum targetEnum = QMetaEnum::fromType<Target>();
    QMetaEnum formatEnum = QMetaEnum::fromType<Format>();
    QMetaEnum typeEnum = QMetaEnum::fromType<Type>();

    Q_ASSERT(targetEnum.isValid());
    Q_ASSERT(formatEnum.isValid());
    Q_ASSERT(typeEnum.isValid());

    const char* targetKey = targetEnum.valueToKey(int(image.target));
    const char* formatKey = formatEnum.valueToKey(int(image.format));
    const char* typeKey = typeEnum.valueToKey(int(image.type));

    if(targetKey == nullptr)
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): invalid value for the target %1.").arg(textureFile.fileName()).arg(int(image.target)));
    if(formatKey == nullptr)
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): invalid value for the format %1.").arg(textureFile.fileName()).arg(int(image.format)));
    if(typeKey == nullptr)
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): invalid type for the format %1.").arg(textureFile.fileName()).arg(int(image.type)));
    if(image.rawDataLength == 0)
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): rawDataLength is not allowed to be 0.").arg(textureFile.fileName()));
    if(image.width == 0)
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): width is not allowed to be 0.").arg(textureFile.fileName()));
    if(image.height == 0)
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): height is not allowed to be 0.").arg(textureFile.fileName()));
    if(image.depth == 0)
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): depth is not allowed to be 0.").arg(textureFile.fileName()));
    if(image.rowStride != image.calcRowStride())
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): rowStride has the wrong value.").arg(textureFile.fileName()));
    if(image.rawDataLength != image.rowStride*image.height*image.depth)
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): rawDataLength does not match the texture dimensions and the internal format of the texture.").arg(textureFile.fileName()));
    if(image.rawDataLength > std::numeric_limits<int>::max())
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): rawDataLength is too long for an array to hold it.").arg(textureFile.fileName()));

    maxMipMap[GLenum(image.target)] = glm::max<GLint>(maxMipMap.value(GLenum(image.target), 0), GLint(image.mipmap));
    minMipMap[GLenum(image.target)] = glm::max<GLint>(minMipMap.value(GLenum(image.target), std::numeric_limits<GLint>::max()), GLint(image.mipmap));

    if(tempBuffer.length() < int(image.rawDataLength))
      tempBuffer.resize(int(image.rawDataLength));
    file.read(reinterpret_cast<char*>(tempBuffer.data()), image.rawDataLength);
    offsetCheck+=image.rawDataLength;

    bool supportedFormat;
    ImportSettings::internalFormat(image.format, image.type, &supportedFormat);
    if(!supportedFormat)
      throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): invalid format type combination of %1 and %2.").arg(textureFile.fileName()).arg(formatKey).arg(typeKey));


    switch(image.target)
    {
    case Target::TEXTURE_1D:
      if(image.width == 0 || image.depth != 1 || image.height!=1)
        throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): a 1d image need layers to have the height 1 and 1 layer. Target: %1, width: %2, height: %3 depth: %4.").arg(textureFile.fileName()).arg(targetKey).arg(image.width).arg(image.height).arg(image.depth));
      break;
    case Target::CUBE_MAP_POSITIVE_X:
    case Target::CUBE_MAP_NEGATIVE_X:
    case Target::CUBE_MAP_POSITIVE_Y:
    case Target::CUBE_MAP_NEGATIVE_Y:
    case Target::CUBE_MAP_POSITIVE_Z:
    case Target::CUBE_MAP_NEGATIVE_Z:
    case Target::TEXTURE_2D:
      if(image.width == 0 || image.height == 0 || image.depth != 1)
        throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): a 2d image need layers to have 1 layer. Target: %1, width: %2, height: %3 depth: %4.").arg(textureFile.fileName()).arg(targetKey).arg(image.width).arg(image.height).arg(image.depth));
      break;
    case Target::TEXTURE_3D:
      if(image.width == 0 || image.height == 0 || image.depth == 0)
        throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): a 1d image need layers to have the height 1 and 1 layer. Target: %1, width: %2, height: %3 depth: %4.").arg(textureFile.fileName()).arg(targetKey).arg(image.width).arg(image.height).arg(image.depth));
      break;
    }

    importedTexture.setUncompressed2DImage(image, tempBuffer.data());
  }

  for(const CompressedImage& image : compressedImages)
  {
    Q_UNUSED(image);
    Q_UNREACHABLE(); // Not implemented yet
  }

  if(quint64(file.size()) != offsetCheck)
    throw GLRT_EXCEPTION(QString("TextureFile::loadFromFile(%0): There are some unused bytes in the texture file").arg(textureFile.fileName()));


  GLuint textureHandle = 0;
  std::swap(textureHandle, importedTexture.textureId);


  for(auto i=maxMipMap.begin(); i!=maxMipMap.end(); ++i)
  {
    GLenum target = i.key();
    GLint maxMipmapLevel = i.value();
    GLint minMipmapLevel = minMipMap[target];

    GL_CALL(glBindTexture, target, textureHandle);

    GL_CALL(glTexParameteri, target, GL_TEXTURE_BASE_LEVEL, minMipmapLevel);
    GL_CALL(glTexParameteri, target, GL_TEXTURE_MAX_LEVEL, maxMipmapLevel);

    GL_CALL(glBindTexture, target, 0);
  }

  return textureHandle;
}

quint32 TextureFile::appendRawData(const QVector<byte>& rawData)
{
  quint32 offset = 0;

  for(const QVector<byte>& rawData : this->rawData)
    offset += quint32(rawData.length());

  this->rawData.append(rawData);

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
  r = angelScriptEngine->RegisterEnumValue("TextureFormat", "RG", int(Format::RG)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureFormat", "RGB", int(Format::RGB)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureFormat", "RGBA", int(Format::RGBA)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterEnum("TextureType"); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "INT8", int(Type::INT8)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "UINT8", int(Type::UINT8)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "INT16", int(Type::INT16)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "UINT16", int(Type::UINT16)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "FLOAT16", int(Type::FLOAT16)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("TextureType", "FLOAT32", int(Type::FLOAT32)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectType(name, sizeof(ImportSettings), AngelScript::asOBJ_VALUE|AngelScript::asOBJ_POD); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("TextureImportSettings", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_constructor<ImportSettings>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureType type", asOFFSET(ImportSettings,type)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureFormat format", asOFFSET(ImportSettings,format)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "int width", asOFFSET(ImportSettings,width)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "int height", asOFFSET(ImportSettings,height)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "bool sourceIsNormalMap", asOFFSET(ImportSettings,sourceIsNormalMap)); AngelScriptCheck(r);
}

quint32 TextureFile::UncompressedImage::calcRowStride() const
{
  GLint packAlignment;
  glGetIntegerv(GL_PACK_ALIGNMENT, &packAlignment);

  quint32 bytesPerPixel = quint32(ImportSettings::bytesPerPixelForFormatType(format, type));

  return quint32(glm::ceilMultiple<quint32>(width * bytesPerPixel, quint32(packAlignment)));
}


} // namespace resources
} // namespace scene
} // namespace glrt
