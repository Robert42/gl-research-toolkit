#include <QMetaEnum>
#include <GL/glew.h>
#include <glm/gtc/round.hpp>
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


struct TextureFile::TextureAsFloats
{
  TextureFile::UncompressedImage image;
  QVector<byte> textureData;
  byte* data = nullptr;
  quint32 w = 0, w_float=0, h = 0;

  TextureAsFloats() = delete;

  TextureAsFloats(quint32 width, quint32 height)
  {
    w = width;
    h = height;
    w_float = w * 4;

    image.type = TextureFile::Type::FLOAT32;
    image.format = TextureFile::Format::RGBA;
    // format and type must be set before calling calcRowStride!

    image.width = w;
    image.rowStride = image.calcRowStride();
    image.height = h;
    image.depth = 1;
    image.mipmap = 0;
    image.target = TextureFile::Target::TEXTURE_2D;
    image.rawDataLength = image.rowStride * image.height;

    textureData.resize(int(this->image.rowStride * h));
    data = reinterpret_cast<byte*>(textureData.data());
  }

  TextureAsFloats(const QImage& qImage)
    : TextureAsFloats(quint32(qImage.width()), quint32(qImage.height()))
  {
    fromQImage(qImage);
  }

  TextureAsFloats(const QPair<UncompressedImage, QVector<byte>>& importedTexture)
  {
    image = importedTexture.first;
    textureData = importedTexture.second;
    data = reinterpret_cast<byte*>(textureData.data());
    w = image.width;
    w_float = w*4;
    h = image.height*image.depth;
  }

  template<typename T>
  T* lineData_As(quint32 y)
  {
    return reinterpret_cast<T*>(data + y * image.rowStride);
  }

  template<typename T>
  const T* lineData_As(quint32 y) const
  {
    return reinterpret_cast<const T*>(data + y * image.rowStride);
  }

  void remapSourceAsSigned()
  {
    #pragma omp parallel for
    for(quint32 y=0; y<h; ++y)
    {
      float* line = this->lineData_As<float>(y);
      for(quint32 x=0; x<w_float; ++x)
        line[x] = line[x]*2.f - 1.f;
    }
  }

  void remapSourceAsUnsigned()
  {
    #pragma omp parallel for
    for(quint32 y=0; y<h; ++y)
    {
      float* line = this->lineData_As<float>(y);
      for(quint32 x=0; x<w_float; ++x)
        line[x] = line[x]*.5f + 0.5f;
    }
  }

  void remap(glm::vec4 offset, glm::vec4 factor)
  {
    #pragma omp parallel for
    for(quint32 y=0; y<h; ++y)
    {
      glm::vec4* line = this->lineData_As<glm::vec4>(y);
      for(quint32 x=0; x<w; ++x)
        line[x] = line[x]*factor + offset;
    }
  }

  inline static float grey(const glm::vec3& x)
  {
    return (x.r + x.g + x.b) / 3.f;
  }

  void mergeWith_as_grey(const TextureAsFloats* red, const TextureAsFloats* green, const TextureAsFloats* blue, const TextureAsFloats* alpha)
  {
    if(!red && !green && !blue && !alpha)
      return;

    if(red)
    {
      Q_ASSERT(red->w == this->w);
      Q_ASSERT(red->h == this->h);
#pragma omp parallel for
      for(quint32 y=0; y<h; ++y)
      {
        glm::vec4* targetLine = this->lineData_As<glm::vec4>(y);
        const glm::vec4* redLine = red->lineData_As<glm::vec4>(y);
        for(quint32 x=0; x<w; ++x)
          targetLine[x].r = grey(redLine[x].rgb());
      }
    }
    if(green)
    {
      Q_ASSERT(green->w == this->w);
      Q_ASSERT(green->h == this->h);
#pragma omp parallel for
      for(quint32 y=0; y<h; ++y)
      {
        glm::vec4* targetLine = this->lineData_As<glm::vec4>(y);
        const glm::vec4* greenLine = green->lineData_As<glm::vec4>(y);
        for(quint32 x=0; x<w; ++x)
          targetLine[x].g = grey(greenLine[x].rgb());
      }
    }
    if(blue)
    {
      Q_ASSERT(blue->w == this->w);
      Q_ASSERT(blue->h == this->h);
#pragma omp parallel for
      for(quint32 y=0; y<h; ++y)
      {
        glm::vec4* targetLine = this->lineData_As<glm::vec4>(y);
        const glm::vec4* blueLine = blue->lineData_As<glm::vec4>(y);
        for(quint32 x=0; x<w; ++x)
          targetLine[x].b = grey(blueLine[x].rgb());
      }
    }
    if(alpha)
    {
      Q_ASSERT(alpha->w == this->w);
      Q_ASSERT(alpha->h == this->h);
#pragma omp parallel for
      for(quint32 y=0; y<h; ++y)
      {
        glm::vec4* targetLine = this->lineData_As<glm::vec4>(y);
        const glm::vec4* alphaLine = alpha->lineData_As<glm::vec4>(y);
        for(quint32 x=0; x<w; ++x)
          targetLine[x].a = grey(alphaLine[x].rgb());
      }
    }
  }

  void mergeWith_channelwise(const TextureAsFloats* red, const TextureAsFloats* green, const TextureAsFloats* blue, const TextureAsFloats* alpha)
  {
    if(!red && !green && !blue && !alpha)
      return;

    Q_ASSERT(red->w == this->w);
    Q_ASSERT(red->h == this->h);
    Q_ASSERT(green->w == this->w);
    Q_ASSERT(green->h == this->h);
    Q_ASSERT(blue->w == this->w);
    Q_ASSERT(blue->h == this->h);
    Q_ASSERT(alpha->w == this->w);
    Q_ASSERT(alpha->h == this->h);

    if(!red)
      red = this;
    if(!green)
      green = this;
    if(!blue)
      blue = this;
    if(!alpha)
      alpha = this;

    #pragma omp parallel for
    for(quint32 y=0; y<h; ++y)
    {
      glm::vec4* targetLine = this->lineData_As<glm::vec4>(y);
      const glm::vec4* redLine = red->lineData_As<glm::vec4>(y);
      const glm::vec4* greenLine = green->lineData_As<glm::vec4>(y);
      const glm::vec4* blueLine = blue->lineData_As<glm::vec4>(y);
      const glm::vec4* alphaLine = alpha->lineData_As<glm::vec4>(y);
      for(quint32 x=0; x<w; ++x)
      {
        targetLine[x].r = redLine[x].r;
        targetLine[x].g = greenLine[x].g;
        targetLine[x].b = blueLine[x].b;
        targetLine[x].a = alphaLine[x].b;
      }
    }
  }

  void mergeWith(const TextureAsFloats* red, const TextureAsFloats* green, const TextureAsFloats* blue, const TextureAsFloats* alpha,
                 bool merge_red_as_grey, bool merge_green_as_grey, bool merge_blue_as_grey, bool merge_alpha_as_grey)
  {
    const TextureAsFloats* redAsGrey = red&&merge_red_as_grey ? red : nullptr;
    const TextureAsFloats* greenAsGrey = green&&merge_green_as_grey ? green : nullptr;
    const TextureAsFloats* blueAsGrey = blue&&merge_blue_as_grey ? blue : nullptr;
    const TextureAsFloats* alphaAsGrey = alpha&&merge_alpha_as_grey ? alpha : nullptr;

    const TextureAsFloats* redChannelwise = red&&!merge_red_as_grey ? red : nullptr;
    const TextureAsFloats* greenChannelwise = green&&!merge_green_as_grey ? green : nullptr;
    const TextureAsFloats* blueChannelwise = blue&&!merge_blue_as_grey ? blue : nullptr;
    const TextureAsFloats* alphaChannelwise = alpha&&!merge_alpha_as_grey ? alpha : nullptr;

    mergeWith_as_grey(redAsGrey, greenAsGrey, blueAsGrey, alphaAsGrey);
    mergeWith_channelwise(redChannelwise, greenChannelwise, blueChannelwise, alphaChannelwise);
  }

  void flipY()
  {
    quint32 half_h = h/2;

#pragma omp parallel for
    for(quint32 y=0; y<half_h; ++y)
    {
      float* srcLine = this->lineData_As<float>(y);
      float* targetLine = this->lineData_As<float>(h-1-y);
      for(quint32 x=0; x<w_float; ++x)
      {
        std::swap(srcLine[x], targetLine[x]);
      }
    }
  }

  void fromQImage(QImage image)
  {
    image = image.convertToFormat(QImage::Format_RGBA8888);

    Q_ASSERT(image.width() == int(w));
    Q_ASSERT(image.height() == int(h));

    const quint8* srcData = reinterpret_cast<quint8*>(image.bits());

#pragma omp parallel for
    for(int y=0; y<int(h); ++y)
    {
      float* targetLine = this->lineData_As<float>(quint32(y));
      const quint8* srcLine = srcData + y*image.bytesPerLine();
      for(int x=0; x<int(w_float); ++x)
        targetLine[x] = srcLine[x] / 255.f;
    }
  }

  QImage asQImage() const
  {
    QImage image(int(w), int(h), QImage::Format_RGBA8888);

    quint8* targetData = reinterpret_cast<quint8*>(image.bits());

#pragma omp parallel for
    for(int y=0; y<int(h); ++y)
    {
      const float* srcLine = this->lineData_As<float>(quint32(y));
      quint8* targetLine = targetData + y*image.bytesPerLine();
      for(int x=0; x<int(w_float); ++x)
        targetLine[x] = static_cast<quint8>(glm::clamp<float>(srcLine[x] * 255.f, 0.f, 255.f));
    }

    return image;
  }

  QImage asChannelQImage(int channel) const
  {
    QImage image(int(w), int(h), QImage::Format_RGBA8888);

    quint8* targetData = reinterpret_cast<quint8*>(image.bits());

#pragma omp parallel for
    for(int y=0; y<int(h); ++y)
    {
      const glm::vec4* srcLine = this->lineData_As<glm::vec4>(quint32(y));
      glm::tvec4<quint8>* targetLine = reinterpret_cast<glm::tvec4<quint8>*>(targetData + y*image.bytesPerLine());
      for(int x=0; x<int(w); ++x)
      {
        quint8 v = static_cast<quint8>(glm::clamp<float>(srcLine[x][channel] * 255.f, 0.f, 255.f));

        targetLine[x].r = v;
        targetLine[x].g = v;
        targetLine[x].b = v;
        targetLine[x].a = 255;
      }
    }

    return image;
  }
};

class TextureFile::ImportedGlTexture
{
public:
  GLuint textureId;

  ImportedGlTexture()
  {
    GL_CALL(glGenTextures, 1, &textureId);
  }

  ImportedGlTexture(const QFileInfo& fileToBeImported, ImportSettings importSettings)
    : ImportedGlTexture()
  {
    QString suffix = fileToBeImported.suffix().toCaseFolded();

    if(suffix == "png")
    {
      QImage image;

      if(!image.load(fileToBeImported.absoluteFilePath()) || image.isNull())
      {
        qDebug() << "Couldn't load image file" << fileToBeImported.absoluteFilePath();
        Q_UNREACHABLE();
      }

      // #TODO resize to power of two?

      TextureAsFloats textureData(image);

      textureData.flipY();

      fromFloats(textureData);

      if(importSettings.generateMipmaps)
        GL_CALL(glGenerateTextureMipmap, textureId);
    }else
    {
      qDebug() << "Unsupported image format" << suffix << "used with the file" << fileToBeImported.absoluteFilePath();
      Q_UNREACHABLE();
    }
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

  GLint maxLevel() const
  {
    for(GLint level=0; level<1000; ++level)
    {
      GLint value;
      GL_CALL(glGetTextureLevelParameteriv, textureId, level, GL_TEXTURE_WIDTH, &value);
      if(value <= 0)
        return level-1;
    }
    return -1;
  }

  Target target() const
  {
    GLint value;
    GL_CALL(glGetTextureParameterIiv, textureId, GL_TEXTURE_TARGET, &value);
    Target target = static_cast<Target>(value);
    switch(target)
    {
    case Target::TEXTURE_1D:
    case Target::TEXTURE_2D:
    case Target::TEXTURE_3D:
    case Target::CUBE_MAP_NEGATIVE_X:
    case Target::CUBE_MAP_NEGATIVE_Y:
    case Target::CUBE_MAP_NEGATIVE_Z:
    case Target::CUBE_MAP_POSITIVE_X:
    case Target::CUBE_MAP_POSITIVE_Y:
    case Target::CUBE_MAP_POSITIVE_Z:
      return target;
    }
    Q_UNREACHABLE();
    return Target::TEXTURE_2D;
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
    image.depth = quint32(this->depth(level));
    image.mipmap = quint32(level);
    image.target = target();
    image.rawDataLength = image.rowStride * image.height * image.depth;

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

  TextureAsFloats asFloats(int level)
  {
    QPair<UncompressedImage, QVector<byte>> importedTexture = uncompressed2DImage(level, Format::RGBA, Type::FLOAT32);

    return importedTexture;
  }

  void fromFloats(const TextureAsFloats& texture)
  {
    setUncompressed2DImage(texture.image, texture.data);
  }
};

TextureFile::TextureFile()
{
}

void TextureFile::appendImage(const QVector<float>& data, const glm::ivec3& size, Target target, Type type, Format format)
{
  quint32 nChannels = quint32(ImportSettings::channelsPerPixelForFormat(format));

  Q_ASSERT(quint32(data.length()) == nChannels*quint32(size.x*size.y*size.z));

  ImportedGlTexture glTex;

  UncompressedImage inputImage;

  inputImage.width = quint32(size.x);
  inputImage.height = quint32(size.y);
  inputImage.depth = quint32(size.z);
  inputImage.format = format;
  inputImage.mipmap = 0;
  inputImage.rawDataLength = quint32(data.length())*sizeof(float);
  inputImage.rawDataStart = 0;
  inputImage.target = target;
  inputImage.type = Type::FLOAT32;
  inputImage.rowStride = inputImage.calcRowStride();

  Q_ASSERT(inputImage.rowStride == nChannels*quint32(size.x)*sizeof(float));

  glTex.setUncompressed2DImage(inputImage, data.data());

  QPair<UncompressedImage, QVector<byte>> image =  glTex.uncompressed2DImage(0, format, type);

  appendUncompressedImage(image.first, image.second);
}

void TextureFile::import(const QFileInfo& srcFile, ImportSettings importSettings)
{
  importSettings.preprocess();

  const std::string sourceFilename = srcFile.absoluteFilePath().toStdString();

  ImportedGlTexture textureInformation(srcFile, importSettings);

  ImportedGlTexture* red_texture = nullptr;
  ImportedGlTexture* green_texture = nullptr;
  ImportedGlTexture* blue_texture = nullptr;
  ImportedGlTexture* alpha_texture = nullptr;

  const int firstMipMap = 0;
  const int lastMipMap = textureInformation.maxLevel();

  if(importSettings.need_merging())
  {
    auto createChannelTextureInfo = [importSettings,&srcFile,&textureInformation](const std::string& suffix) {
      ImportedGlTexture* tex = nullptr;
      if(!suffix.empty())
      {
        QFileInfo newFile = QDir(srcFile.path()).filePath(srcFile.baseName().replace(QRegularExpression("[-_][^-_]+$"), QString::fromStdString(suffix)));
        if(!newFile.exists())
          throw GLRT_EXCEPTION(QString("The file %0 doesn't exist.").arg(newFile.filePath()));
        tex = new ImportedGlTexture(newFile, importSettings);
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
