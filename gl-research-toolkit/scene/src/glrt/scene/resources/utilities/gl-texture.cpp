#include <GL/glew.h>
#include <glrt/scene/resources/utilities/gl-texture.h>

#include <QImage>

namespace glrt {
namespace scene {
namespace resources {
namespace utilities {


int GlTexture::channelsPerPixelForFormat(Format format)
{
  switch(format)
  {
  case Format::RED:
  case Format::RED_INTEGER:
    return 1;
  case Format::RG_INTEGER:
  case Format::RG:
    return 2;
  case Format::RGB_INTEGER:
  case Format::RGB:
    return 3;
  case Format::RGBA_INTEGER:
  case Format::RGBA:
    return 4;
  }
  Q_UNREACHABLE();
}

int GlTexture::bytesPerPixelForType(Type type)
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

int GlTexture::bytesPerPixelForFormatType(Format format, Type type)
{
  return channelsPerPixelForFormat(format) * bytesPerPixelForType(type);
}

quint32 GlTexture::UncompressedImage::calcRowStride() const
{
  Q_ASSERT(alignment==1 || alignment==2 || alignment==4 || alignment==8);

  quint32 bytesPerPixel = quint32(bytesPerPixelForFormatType(format, type));

  return quint32(glm::ceilMultiple<quint32>(width * bytesPerPixel, alignment));
}

GLenum GlTexture::internalFormat(Format format, Type type, bool* supported)
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
    case Format::RED_INTEGER:
      return GL_R8UI;
    case Format::RG_INTEGER:
      return GL_RG8UI;
    case Format::RGB_INTEGER:
      return GL_RGB8UI;
    case Format::RGBA_INTEGER:
      return GL_RGBA8UI;
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
    case Format::RED_INTEGER:
      return GL_R8I;
    case Format::RG_INTEGER:
      return GL_RG8I;
    case Format::RGB_INTEGER:
      return GL_RGB8I;
    case Format::RGBA_INTEGER:
      return GL_RGBA8I;
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
      break; // Why?
    case Format::RGBA:
      return GL_RGBA16;
    case Format::RED_INTEGER:
      return GL_R16UI;
    case Format::RG_INTEGER:
      return GL_RG16UI;
    case Format::RGB_INTEGER:
      return GL_RGB16UI;
    case Format::RGBA_INTEGER:
      return GL_RGBA16UI;
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
    case Format::RED_INTEGER:
      return GL_R16I;
    case Format::RG_INTEGER:
      return GL_RG16I;
    case Format::RGB_INTEGER:
      return GL_RGB16I;
    case Format::RGBA_INTEGER:
      return GL_RGBA16I;
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
    case Format::RED_INTEGER:
    case Format::RG_INTEGER:
    case Format::RGB_INTEGER:
    case Format::RGBA_INTEGER:
      break;
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
    case Format::RED_INTEGER:
    case Format::RG_INTEGER:
    case Format::RGB_INTEGER:
    case Format::RGBA_INTEGER:
      break;
    }
    break;
  }
  Q_UNREACHABLE();
  *supported = false;
  return GL_R8;
}



GlTexture::UncompressedImage GlTexture::TextureAsFloats::format(quint32 width, quint32 height, quint32 depth, quint32 numComponents)
{
  UncompressedImage image;

  switch(numComponents)
  {
  case 4:
    image.format = Format::RGBA;
    break;
  case 3:
    image.format = Format::RGB;
    break;
  case 2:
    image.format = Format::RG;
    break;
  case 1:
    image.format = Format::RED;
    break;
  default:
    Q_UNREACHABLE();
  }
  image.type = Type::FLOAT32;
  // format and type must be set before calling calcRowStride!

  quint32 rowCount = height * depth;

  image.width = width;
  image.alignment = 1;
  image.rowStride = image.calcRowStride();
  image.height = height;
  image.depth = depth;
  image.mipmap = 0;
  image.target = image.depth==1 ? Target::TEXTURE_2D : Target::TEXTURE_3D;
  image.rawDataLength = image.rowStride * rowCount;

  return image;
}

GlTexture::UncompressedImage GlTexture::TextureAsFloats::format(const glm::ivec3& textureSize, quint32 numComponents)
{
  return format(textureSize.x, textureSize.y, textureSize.z, numComponents);
}

GlTexture::TextureAsFloats::TextureAsFloats(quint32 width, quint32 height, quint32 depth, quint32 numComponents)
{
  this->width = width;
  this->height = height;
  this->depth = depth;
  this->components_per_row = width*numComponents;
  this->rowCount = height*depth;

  this->image = format(width, height, depth, numComponents);

  textureData.resize(int(this->components_per_row * rowCount));
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
  textureData.resize((importedTexture.second.length()+sizeof(float)-1)/sizeof(float));
  std::memcpy(textureData.data(), importedTexture.second.data(), importedTexture.second.length());
  data = reinterpret_cast<byte*>(textureData.data());
  width = image.width;
  components_per_row = width*4;
  height = image.height;
  depth = image.depth;
  rowCount = height * depth;
}

GlTexture::TextureAsFloats::TextureAsFloats(const glm::ivec3& size, quint32 numComponents)
  : TextureAsFloats(size.x, size.y, size.z, numComponents)
{
}


void GlTexture::TextureAsFloats::remapSourceAsSigned()
{
  #pragma omp parallel for
  for(quint32 y=0; y<rowCount; ++y)
  {
    float* line = this->lineData_As<float>(y);
    for(quint32 x=0; x<components_per_row; ++x)
      line[x] = line[x]*2.f - 1.f;
  }
}

void GlTexture::TextureAsFloats::remapSourceAsUnsigned()
{
  #pragma omp parallel for
  for(quint32 y=0; y<rowCount; ++y)
  {
    float* line = this->lineData_As<float>(y);
    for(quint32 x=0; x<components_per_row; ++x)
      line[x] = line[x]*.5f + 0.5f;
  }
}

void GlTexture::TextureAsFloats::remap(glm::vec4 offset, glm::vec4 factor)
{
  #pragma omp parallel for
  for(quint32 y=0; y<rowCount; ++y)
  {
    glm::vec4* line = this->lineData_As<glm::vec4>(y);
    for(quint32 x=0; x<width; ++x)
      line[x] = line[x]*factor + offset;
  }
}


void GlTexture::TextureAsFloats::mergeWith_as_grey(const TextureAsFloats* red, const TextureAsFloats* green, const TextureAsFloats* blue, const TextureAsFloats* alpha)
{
  if(!red && !green && !blue && !alpha)
    return;

  if(red)
  {
    Q_ASSERT(red->width == this->width);
    Q_ASSERT(red->rowCount == this->rowCount);
#pragma omp parallel for
    for(quint32 y=0; y<rowCount; ++y)
    {
      glm::vec4* targetLine = this->lineData_As<glm::vec4>(y);
      const glm::vec4* redLine = red->lineData_As<glm::vec4>(y);
      for(quint32 x=0; x<rowCount; ++x)
        targetLine[x].r = grey(redLine[x].rgb());
    }
  }
  if(green)
  {
    Q_ASSERT(green->width == this->width);
    Q_ASSERT(green->rowCount == this->rowCount);
#pragma omp parallel for
    for(quint32 y=0; y<rowCount; ++y)
    {
      glm::vec4* targetLine = this->lineData_As<glm::vec4>(y);
      const glm::vec4* greenLine = green->lineData_As<glm::vec4>(y);
      for(quint32 x=0; x<width; ++x)
        targetLine[x].g = grey(greenLine[x].rgb());
    }
  }
  if(blue)
  {
    Q_ASSERT(blue->width == this->width);
    Q_ASSERT(blue->rowCount == this->rowCount);
#pragma omp parallel for
    for(quint32 y=0; y<rowCount; ++y)
    {
      glm::vec4* targetLine = this->lineData_As<glm::vec4>(y);
      const glm::vec4* blueLine = blue->lineData_As<glm::vec4>(y);
      for(quint32 x=0; x<width; ++x)
        targetLine[x].b = grey(blueLine[x].rgb());
    }
  }
  if(alpha)
  {
    Q_ASSERT(alpha->width == this->width);
    Q_ASSERT(alpha->rowCount == this->rowCount);
#pragma omp parallel for
    for(quint32 y=0; y<rowCount; ++y)
    {
      glm::vec4* targetLine = this->lineData_As<glm::vec4>(y);
      const glm::vec4* alphaLine = alpha->lineData_As<glm::vec4>(y);
      for(quint32 x=0; x<width; ++x)
        targetLine[x].a = grey(alphaLine[x].rgb());
    }
  }
}

void GlTexture::TextureAsFloats::mergeWith_channelwise(const TextureAsFloats* red, const TextureAsFloats* green, const TextureAsFloats* blue, const TextureAsFloats* alpha)
{
  if(!red && !green && !blue && !alpha)
    return;

  Q_ASSERT(red->width == this->width);
  Q_ASSERT(red->rowCount == this->rowCount);
  Q_ASSERT(green->width == this->width);
  Q_ASSERT(green->rowCount == this->rowCount);
  Q_ASSERT(blue->width == this->width);
  Q_ASSERT(blue->rowCount == this->rowCount);
  Q_ASSERT(alpha->width == this->width);
  Q_ASSERT(alpha->rowCount == this->rowCount);

  if(!red)
    red = this;
  if(!green)
    green = this;
  if(!blue)
    blue = this;
  if(!alpha)
    alpha = this;

  #pragma omp parallel for
  for(quint32 y=0; y<rowCount; ++y)
  {
    glm::vec4* targetLine = this->lineData_As<glm::vec4>(y);
    const glm::vec4* redLine = red->lineData_As<glm::vec4>(y);
    const glm::vec4* greenLine = green->lineData_As<glm::vec4>(y);
    const glm::vec4* blueLine = blue->lineData_As<glm::vec4>(y);
    const glm::vec4* alphaLine = alpha->lineData_As<glm::vec4>(y);
    for(quint32 x=0; x<width; ++x)
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
  quint32 half_h = rowCount/2;

#pragma omp parallel for
  for(quint32 y=0; y<half_h; ++y)
  {
    float* srcLine = this->lineData_As<float>(y);
    float* targetLine = this->lineData_As<float>(rowCount-1-y);
    for(quint32 x=0; x<components_per_row; ++x)
    {
      std::swap(srcLine[x], targetLine[x]);
    }
  }
}

void GlTexture::TextureAsFloats::fromQImage(QImage image)
{
  image = image.convertToFormat(QImage::Format_RGBA8888);

  Q_ASSERT(image.width() == int(width));
  Q_ASSERT(image.height() == int(height));
  Q_ASSERT(1 == int(depth));

  const quint8* srcData = reinterpret_cast<quint8*>(image.bits());

#pragma omp parallel for
  for(int y=0; y<int(rowCount); ++y)
  {
    float* targetLine = this->lineData_As<float>(quint32(y));
    const quint8* srcLine = srcData + y*image.bytesPerLine();
    for(int x=0; x<int(components_per_row); ++x)
      targetLine[x] = srcLine[x] / 255.f;
  }
}

QImage GlTexture::TextureAsFloats::asQImage() const
{
  QImage image(int(width), int(height), QImage::Format_RGBA8888);

  quint8* targetData = reinterpret_cast<quint8*>(image.bits());

#pragma omp parallel for
  for(int y=0; y<int(height); ++y)
  {
    const float* srcLine = this->lineData_As<float>(quint32(y));
    quint8* targetLine = targetData + y*image.bytesPerLine();
    for(int x=0; x<int(components_per_row); ++x)
      targetLine[x] = static_cast<quint8>(glm::clamp<float>(srcLine[x] * 255.f, 0.f, 255.f));
  }

  return image;
}

QImage GlTexture::TextureAsFloats::asChannelQImage(int channel) const
{
  QImage image(int(width), int(height), QImage::Format_RGBA8888);

  quint8* targetData = reinterpret_cast<quint8*>(image.bits());

#pragma omp parallel for
  for(int y=0; y<int(height); ++y)
  {
    const glm::vec4* srcLine = this->lineData_As<glm::vec4>(quint32(y));
    glm::tvec4<quint8>* targetLine = reinterpret_cast<glm::tvec4<quint8>*>(targetData + y*image.bytesPerLine());
    for(int x=0; x<int(width); ++x)
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

GlTexture::UncompressedImage GlTexture::format(const glm::uvec3& size, int level, GlTexture::Format format, GlTexture::Type type, Target target)
{
  UncompressedImage image;
  image.type = type;
  image.format = format;
  image.alignment = 1;

  image.width = size.x;
  image.height = size.y;
  image.depth = size.z;

  // format, type, width and alignment must be set before calling calcRowStride!
  image.rowStride = image.calcRowStride();
  image.mipmap = quint32(level);
  image.target = target;
  image.rawDataLength = image.rowStride * image.height * image.depth;
  bool supportedFormat;
  internalFormat(image.format, image.type, &supportedFormat);
  Q_ASSERT(supportedFormat);

  return image;
}

QPair<GlTexture::UncompressedImage, QVector<byte>> GlTexture::uncompressed2DImage(int level,
                                                                                  Format format,
                                                                                  Type type) const
{
  UncompressedImage image;

  image.type = type;
  image.format = format;
  image.alignment = 1;
  image.width = quint32(this->width(level));
  // format, type, width and alignment must be set before calling calcRowStride!

  image.rowStride = image.calcRowStride();
  image.height = quint32(this->height(level));
  image.depth = quint32(this->depth(level));
  image.mipmap = quint32(level);
  image.target = target();
  image.rawDataLength = image.rowStride * image.height * image.depth;

  bool supportedFormat;
  internalFormat(image.format, image.type, &supportedFormat);
  Q_ASSERT(supportedFormat);

  QVector<byte> rawData;
  rawData.resize(int(image.rawDataLength));

  GL_CALL(glPixelStorei, GL_PACK_ALIGNMENT, image.alignment);

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
  GLenum internalFormat = GlTexture::internalFormat(image.format, image.type, &supportedFormat);
  Q_ASSERT(supportedFormat);

  GL_CALL(glBindTexture, static_cast<GLenum>(image.target), this->textureId);

  GL_CALL(glPixelStorei, GL_UNPACK_ALIGNMENT, image.alignment);

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
