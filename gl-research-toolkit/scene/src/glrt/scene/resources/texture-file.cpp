#include <QMetaEnum>
#include <GL/glew.h>
#include <glrt/scene/resources/texture-file.h>
#include <glrt/toolkit/plain-old-data-stream.h>
#include <angelscript-integration/collection-converter.h>
#include <angelscript-integration/ref-counted-object.h>
#include <QLabel>
#include <QDialog>
#include <QImage>
#include <QVBoxLayout>

namespace glrt {
namespace scene {
namespace resources {

#define SHOW_TEXTURE_IN_DIALOG 0

using AngelScriptIntegration::AngelScriptCheck;


TextureFile::TextureFile()
{
}

void TextureFile::appendImage(const GlTexture& texture, Type type, Format format)
{
  QPair<UncompressedImage, QVector<byte>> image =  texture.uncompressed2DImage(0, format, type);
  appendUncompressedImage(image.first, image.second);
}

void TextureFile::import(const QFileInfo& srcFile, ImportSettings importSettings)
{
  importSettings.preprocess();

  const std::string sourceFilename = srcFile.absoluteFilePath().toStdString();

  GlTexture textureInformation(srcFile, importSettings);

  GlTexture* red_texture = nullptr;
  GlTexture* green_texture = nullptr;
  GlTexture* blue_texture = nullptr;
  GlTexture* alpha_texture = nullptr;

  const int firstMipMap = 0;
  const int lastMipMap = textureInformation.maxLevel();

  if(importSettings.need_merging())
  {
    auto createChannelTextureInfo = [importSettings,&srcFile,&textureInformation](const std::string& suffix) {
      GlTexture* tex = nullptr;
      if(!suffix.empty())
      {
        QFileInfo newFile = QDir(srcFile.path()).filePath(srcFile.baseName().replace(QRegularExpression("[-_][^-_]+$"), QString::fromStdString(suffix)));
        if(!newFile.exists())
          throw GLRT_EXCEPTION(QString("The file %0 doesn't exist.").arg(newFile.filePath()));
        tex = new GlTexture(newFile, importSettings);
        if(tex->width(0) != textureInformation.width(0) || tex->height(0) != textureInformation.height(0))
          throw GLRT_EXCEPTION(QString("Merged Texture import: mismatched texture size between %0 and %1. (the channel texture has the size %2x%3 and the original tetxure has the size %4x%5)").arg(srcFile.filePath()).arg(newFile.filePath()).arg(tex->width(0)).arg(tex->height(0)).arg(textureInformation.width(0)).arg(textureInformation.height(0)));
      }
      return tex;
    };

    red_texture = createChannelTextureInfo(importSettings.red_channel_suffix);
    green_texture = createChannelTextureInfo(importSettings.green_channel_suffix);
    blue_texture = createChannelTextureInfo(importSettings.blue_channel_suffix);
    alpha_texture = createChannelTextureInfo(importSettings.alpha_channel_suffix);
  }

  if(importSettings.compression == TextureFile::Compression::NONE)
  {
    for(int level=firstMipMap; level<=lastMipMap; ++level)
    {
      if(importSettings.need_processing())
      {
        TextureAsFloats asFloats = textureInformation.asFloats(level);

        if(importSettings.need_merging())
        {
          const TextureAsFloats* redAsFloats = nullptr;
          const TextureAsFloats* greenAsFloats = nullptr;
          const TextureAsFloats* blueAsFloats = nullptr;
          const TextureAsFloats* alphaAsFloats = nullptr;

          if(red_texture)
            redAsFloats = new TextureAsFloats(red_texture->asFloats(level));
          if(green_texture)
            greenAsFloats = new TextureAsFloats(green_texture->asFloats(level));
          if(blue_texture)
            blueAsFloats = new TextureAsFloats(blue_texture->asFloats(level));
          if(alpha_texture)
            alphaAsFloats = new TextureAsFloats(alpha_texture->asFloats(level));

          asFloats.mergeWith(redAsFloats,
                             greenAsFloats,
                             blueAsFloats,
                             alphaAsFloats,
                             importSettings.merge_red_as_grey,
                             importSettings.merge_green_as_grey,
                             importSettings.merge_blue_as_grey,
                             importSettings.merge_alpha_as_grey);

          delete redAsFloats;
          delete greenAsFloats;
          delete blueAsFloats;
          delete alphaAsFloats;
        }

        if(importSettings.remapSourceAsSigned)
          asFloats.remapSourceAsSigned();

        if(importSettings.need_remapping())
          asFloats.remap(importSettings.offset, importSettings.factor);

        textureInformation.fromFloats(asFloats);
      }

      QPair<UncompressedImage, QVector<byte>> image =  textureInformation.uncompressed2DImage(level, importSettings.format, importSettings.type);

      appendUncompressedImage(image.first, image.second);
    }

#if SHOW_TEXTURE_IN_DIALOG
    {
      TextureAsFloats _asFloats = textureInformation.asFloats(1);
      glm::vec4 smallestValue = glm::vec4(INFINITY);
      glm::vec4 largestValue = glm::vec4(-INFINITY);
      for(quint32 y=0; y<_asFloats.h; ++y)
      {
        glm::vec4* line = _asFloats.lineData_As<glm::vec4>(y);
        for(quint32 x=0; x<_asFloats.w; ++x)
        {
          smallestValue = glm::min(line[x], smallestValue);
          largestValue = glm::max(line[x], largestValue);
        }
      }
      QImage debugImage = _asFloats.asChannelQImage(3);
      //            QImage debugImage = _asFloats.asQImage();
      QDialog texturePreview;
      QVBoxLayout vbox(&texturePreview);
      QLabel label;
      vbox.addWidget(&label);
      qDebug() << "min: " << smallestValue << "max: " << largestValue;
      label.setPixmap(QPixmap::fromImage(debugImage));
      label.setVisible(true);
      texturePreview.exec();
    }
#endif
  }else
  {
    Q_UNREACHABLE();
  }

  delete red_texture;
  delete green_texture;
  delete blue_texture;
  delete alpha_texture;
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


  GlTexture importedTexture;

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
    minMipMap[GLenum(image.target)] = glm::min<GLint>(minMipMap.value(GLenum(image.target), std::numeric_limits<GLint>::max()), GLint(image.mipmap));

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

void TextureFile::appendUncompressedImage(UncompressedImage image, const QVector<byte>& rawData)
{
  if(image.rawDataLength == 0)
    image.rawDataLength = quint32(rawData.length());
  Q_ASSERT(image.rawDataLength == quint32(rawData.length()));
  image.rawDataStart = this->appendRawData(rawData);

  this->uncompressedImages.append(image);
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
  return magicNumberForString("glrt-tex");
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

  r = angelScriptEngine->RegisterObjectType(name, sizeof(ImportSettings), AngelScript::asOBJ_VALUE|AngelScript::asOBJ_APP_CLASS_CDAK); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("TextureImportSettings", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_constructor<ImportSettings>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("TextureImportSettings", AngelScript::asBEHAVE_DESTRUCT, "void f()", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_destructor<ImportSettings>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("TextureImportSettings", "void opAssign(const TextureImportSettings &in other)", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_assign_operator<ImportSettings>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureType type", asOFFSET(ImportSettings,type)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "TextureFormat format", asOFFSET(ImportSettings,format)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "bool remapSourceAsSigned", asOFFSET(ImportSettings,remapSourceAsSigned)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "bool generateMipmaps", asOFFSET(ImportSettings,generateMipmaps)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "bool scaleDownToPowerOfTwo", asOFFSET(ImportSettings,scaleDownToPowerOfTwo)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "vec4 offset", asOFFSET(ImportSettings,offset)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "vec4 factor", asOFFSET(ImportSettings,factor)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "string red_channel_suffix", asOFFSET(ImportSettings,red_channel_suffix)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "string green_channel_suffix", asOFFSET(ImportSettings,green_channel_suffix)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "string blue_channel_suffix", asOFFSET(ImportSettings,blue_channel_suffix)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "string alpha_channel_suffix", asOFFSET(ImportSettings,alpha_channel_suffix)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "bool merge_red_as_grey", asOFFSET(ImportSettings,merge_red_as_grey)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "bool merge_green_as_grey", asOFFSET(ImportSettings,merge_green_as_grey)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "bool merge_blue_as_grey", asOFFSET(ImportSettings,merge_blue_as_grey)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "bool merge_alpha_as_grey", asOFFSET(ImportSettings,merge_alpha_as_grey)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "vec4 force_value", asOFFSET(ImportSettings,force_value)); AngelScriptCheck(r);
}


} // namespace resources
} // namespace scene
} // namespace glrt
