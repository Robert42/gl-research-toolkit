#ifndef GLRT_SCENE_RESOURCES_TEXTUREFILE_H
#define GLRT_SCENE_RESOURCES_TEXTUREFILE_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/array.h>
#include <glhelper/texture2d.hpp>
#include <glhelper/texture3d.hpp>

#include <QFileInfo>

namespace glrt {
namespace scene {
namespace resources {

class TextureFile
{
public:
  struct ImportSettings
  {
    // See https://www.opengl.org/sdk/docs/man/html/glGetTexImage.xhtml for supported formats
    enum class Format : quint32
    {
      RED  = GL_RED,
      GREEN = GL_GREEN,
      BLUE = GL_BLUE,
      RG = GL_RG,
      RGB = GL_RGB,
      RGBA = GL_RGBA,
      BGR = GL_BGR,
      BGRA = GL_BGRA,
    };
    // See https://www.opengl.org/sdk/docs/man/html/glGetTexImage.xhtml for supported types
    enum class Type : quint32
    {
      UINT8 = GL_UNSIGNED_BYTE,
      INT8 = GL_BYTE,
      UINT16 = GL_UNSIGNED_SHORT,
      INT16 = GL_SHORT,
      UINT32 = GL_UNSIGNED_INT,
      INT32 = GL_INT,
      FLOAT16 = GL_HALF_FLOAT,
      FLOAT32 = GL_FLOAT,
    };

    enum class Compression : quint32
    {
      NONE,
    };

    enum class Target : quint32
    {
      TEXTURE_2D = GL_TEXTURE_2D,
    };

    static int channelsPerPixelForFormat(Format format);
    static int bytesPerPixelForType(Type type);
    static int bytesPerPixelForFormatType(Format format, Type type);

    Target target = Target::TEXTURE_2D;
    Type type = Type::UINT8;
    Format format = Format::BGRA;
    Compression compression = Compression::NONE;
    int width = -1;
    int height = -1;

    // #TODO test, whether the sampler code in the shader accepts signed bytes:
    int sourceIsNormalMap = false;

    static void registerType();
  };

  TextureFile();

  void import(QFileInfo& srcFile, const ImportSettings& importSettings);
  void save(QFileInfo& textureFile);

private:
  class ImportedGlTexture;

  struct Header
  {
    quint64 magicNumber = TextureFile::magicNumber();
    quint16 headerLength = sizeof(Header);
    quint16 uncompressedImageSize = sizeof(UncompressedImage);
    quint16 compressedImageSize = 0;
    quint16 numUncompressedImages = 0;
    quint16 numCompressedImages = 0;
    padding<quint16, 3> _padding;
  };

  struct UncompressedImage
  {
    quint32 rowStride, width, height, layers, mipmap;
    ImportSettings::Target target;
    ImportSettings::Format format;
    ImportSettings::Type type;
    quint32 rawDataStart;
    quint32 rawDataLength;
  };

  struct CompressedImage
  {
    quint32 rawDataStart;
    quint32 rawDataLength;
  };

  QVector<CompressedImage> compressedImages;
  QVector<UncompressedImage> uncompressedImages;
  QMap<quint32, QVector<byte>> rawData;

  quint32 appendRawData(const QVector<byte>& rawData);

  void clear();

  static quint64 magicNumber();
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_TEXTUREFILE_H
