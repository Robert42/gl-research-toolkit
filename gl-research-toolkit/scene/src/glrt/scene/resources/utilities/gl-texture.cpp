#include <GL/glew.h>
#include <glrt/scene/resources/utilities/gl-texture.h>

#include <QImage>

namespace glrt {
namespace scene {
namespace resources {
namespace utilities {


int GlTexture::ImportSettings::channelsPerPixelForFormat(Format format)
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

int GlTexture::ImportSettings::bytesPerPixelForType(Type type)
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

int GlTexture::ImportSettings::bytesPerPixelForFormatType(Format format, Type type)
{
  return channelsPerPixelForFormat(format) * bytesPerPixelForType(type);
}

quint32 GlTexture::UncompressedImage::calcRowStride() const
{
  GLint packAlignment;
  glGetIntegerv(GL_PACK_ALIGNMENT, &packAlignment);

  quint32 bytesPerPixel = quint32(ImportSettings::bytesPerPixelForFormatType(format, type));

  return quint32(glm::ceilMultiple<quint32>(width * bytesPerPixel, quint32(packAlignment)));
}

GLenum GlTexture::ImportSettings::internalFormat(Format format, Type type, bool* supported)
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



GlTexture::TextureAsFloats::TextureAsFloats(quint32 width, quint32 height)
{
  w = width;
  h = height;
  w_float = w * 4;

  image.type = Type::FLOAT32;
  image.format = Format::RGBA;
  // format and type must be set before calling calcRowStride!

  image.width = w;
  image.rowStride = image.calcRowStride();
  image.height = h;
  image.depth = 1;
  image.mipmap = 0;
  image.target = Target::TEXTURE_2D;
  image.rawDataLength = image.rowStride * image.height;

  textureData.resize(int(this->image.rowStride * h));
  data = reinterpret_cast<byte*>(textureData.data());
}

GlTexture::TextureAsFloats::TextureAsFloats(const QImage& qImage)
  : TextureAsFloats(quint32(qImage.width()), quint32(qImage.height()))
{
  fromQImage(qImage);
}

GlTexture::TextureAsFloats::TextureAsFloats(const QPair<UncompressedImage, QVector<byte>>& importedTexture)
{
  image = importedTexture.first;
  textureData = importedTexture.second;
  data = reinterpret_cast<byte*>(textureData.data());
  w = image.width;
  w_float = w*4;
  h = image.height*image.depth;
}


void GlTexture::TextureAsFloats::remapSourceAsSigned()
{
  #pragma omp parallel for
  for(quint32 y=0; y<h; ++y)
  {
    float* line = this->lineData_As<float>(y);
    for(quint32 x=0; x<w_float; ++x)
      line[x] = line[x]*2.f - 1.f;
  }
}

void GlTexture::TextureAsFloats::remapSourceAsUnsigned()
{
  #pragma omp parallel for
  for(quint32 y=0; y<h; ++y)
  {
    float* line = this->lineData_As<float>(y);
    for(quint32 x=0; x<w_float; ++x)
      line[x] = line[x]*.5f + 0.5f;
  }
}

void GlTexture::TextureAsFloats::remap(glm::vec4 offset, glm::vec4 factor)
{
  #pragma omp parallel for
  for(quint32 y=0; y<h; ++y)
  {
    glm::vec4* line = this->lineData_As<glm::vec4>(y);
    for(quint32 x=0; x<w; ++x)
      line[x] = line[x]*factor + offset;
  }
}


void GlTexture::TextureAsFloats::mergeWith_as_grey(const TextureAsFloats* red, const TextureAsFloats* green, const TextureAsFloats* blue, const TextureAsFloats* alpha)
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

void GlTexture::TextureAsFloats::mergeWith_channelwise(const TextureAsFloats* red, const TextureAsFloats* green, const TextureAsFloats* blue, const TextureAsFloats* alpha)
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

void GlTexture::TextureAsFloats::mergeWith(const TextureAsFloats* red, const TextureAsFloats* green, const TextureAsFloats* blue, const TextureAsFloats* alpha,
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

void GlTexture::TextureAsFloats::flipY()
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

void GlTexture::TextureAsFloats::fromQImage(QImage image)
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

QImage GlTexture::TextureAsFloats::asQImage() const
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

QImage GlTexture::TextureAsFloats::asChannelQImage(int channel) const
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



GlTexture::GlTexture()
{
  GL_CALL(glGenTextures, 1, &textureId);
}

GlTexture::GlTexture(const QFileInfo& fileToBeImported, ImportSettings importSettings)
  : GlTexture()
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

    if(importSettings.scaleDownToPowerOfTwo && (!glm::isPowerOfTwo(image.width()) || !glm::isPowerOfTwo(image.height())))
      image = image.scaled(glm::floorPowerOfTwo<int>(image.width()),
                           glm::floorPowerOfTwo<int>(image.height()),
                           Qt::IgnoreAspectRatio,
                           Qt::SmoothTransformation);

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

GlTexture::GlTexture(GlTexture&& other)
  : textureId(other.textureId)
{
  GL_CALL(glGenTextures, 1, &other.textureId);
}

void GlTexture::operator=(GlTexture&& other)
{
  std::swap(this->textureId, other.textureId);
}

GlTexture::~GlTexture()
{
  GL_CALL(glDeleteTextures, 1, &textureId);
}

GLint GlTexture::width(int level) const
{
  Q_ASSERT(level >= 0);
  Q_ASSERT(level < 65536);

  GLint value;
  GL_CALL(glGetTextureLevelParameteriv, textureId, level, GL_TEXTURE_WIDTH, &value);
  Q_ASSERT(value > 0);
  Q_ASSERT(value < 65536);
  return value;
}

GLint GlTexture::height(int level) const
{
  Q_ASSERT(level >= 0);
  Q_ASSERT(level < 65536);

  GLint value;
  GL_CALL(glGetTextureLevelParameteriv, textureId, level, GL_TEXTURE_HEIGHT, &value);
  Q_ASSERT(value > 0);
  Q_ASSERT(value < 65536);
  return value;
}

GLint GlTexture::depth(int level) const
{
  Q_ASSERT(level >= 0);
  Q_ASSERT(level < 65536);

  GLint value;
  GL_CALL(glGetTextureLevelParameteriv, textureId, level, GL_TEXTURE_DEPTH, &value);
  Q_ASSERT(value > 0);
  Q_ASSERT(value < 65536);
  return value;
}

GLint GlTexture::maxLevel() const
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

GlTexture::Target GlTexture::target() const
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

QPair<GlTexture::UncompressedImage, QVector<byte>> GlTexture::uncompressed2DImage(int level,
                                                                                  Format format,
                                                                                  Type type) const
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

void GlTexture::setUncompressed2DImage(const GlTexture::UncompressedImage& image, const void* data)
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

GlTexture::TextureAsFloats GlTexture::asFloats(int level)
{
  QPair<UncompressedImage, QVector<byte>> importedTexture = uncompressed2DImage(level, Format::RGBA, Type::FLOAT32);

  return importedTexture;
}

void GlTexture::fromFloats(const TextureAsFloats& texture)
{
  setUncompressed2DImage(texture.image, texture.data);
}



} // namespace utilities
} // namespace resources
} // namespace scene
} // namespace glrt
