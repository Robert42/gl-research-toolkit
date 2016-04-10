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

class TextureFile : public QObject
{
  Q_OBJECT
public:
  // See https://www.opengl.org/sdk/docs/man/html/glGetTexImage.xhtml for supported formats
  enum class Format : quint32
  {
    RED  = GL_RED,
    RG = GL_RG,
    RGB = GL_RGB,
    RGBA = GL_RGBA,
  };
  // See https://www.opengl.org/sdk/docs/man/html/glGetTexImage.xhtml for supported types
  enum class Type : quint32
  {
    UINT8 = GL_UNSIGNED_BYTE,
    INT8 = GL_BYTE,
    UINT16 = GL_UNSIGNED_SHORT,
    INT16 = GL_SHORT,
    FLOAT16 = GL_HALF_FLOAT,
    FLOAT32 = GL_FLOAT,
  };

  enum class Compression : quint32
  {
    NONE,
  };

  enum class Target : quint32
  {
    TEXTURE_1D = GL_TEXTURE_1D,
    TEXTURE_2D = GL_TEXTURE_2D,
    TEXTURE_3D = GL_TEXTURE_3D,
    CUBE_MAP_POSITIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    CUBE_MAP_NEGATIVE_X = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    CUBE_MAP_POSITIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    CUBE_MAP_NEGATIVE_Y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    CUBE_MAP_POSITIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    CUBE_MAP_NEGATIVE_Z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
  };

  Q_ENUM(Format)
  Q_ENUM(Type)
  Q_ENUM(Compression)
  Q_ENUM(Target)

  struct ImportSettings
  {
    static int channelsPerPixelForFormat(Format format);
    static int bytesPerPixelForType(Type type);
    static int bytesPerPixelForFormatType(Format format, Type type);
    static GLenum internalFormat(Format format, Type type, bool* supported=nullptr);

    Target target = Target::TEXTURE_2D;
    Type type = Type::UINT8;
    Format format = Format::RGBA;
    Compression compression = Compression::NONE;
    bool remapSourceAsSigned = false;
    bool generateMipmaps = true;
    glm::vec4 offset = glm::vec4(0);
    glm::vec4 factor = glm::vec4(1);
    std::string red_channel_suffix;
    std::string green_channel_suffix;
    std::string blue_channel_suffix;
    std::string alpha_channel_suffix;
    bool merge_red_as_grey = true;
    bool merge_green_as_grey = true;
    bool merge_blue_as_grey = true;
    bool merge_alpha_as_grey = true;
    glm::vec4 force_value = glm::vec4(NAN);

    void preprocess()
    {
      if(!glm::isnan(force_value.r))
      {
        offset.r = force_value.r;
        factor.r = 0;
      }
      if(!glm::isnan(force_value.g))
      {
        offset.g = force_value.g;
        factor.g = 0;
      }
      if(!glm::isnan(force_value.b))
      {
        offset.b = force_value.b;
        factor.b = 0;
      }
      if(!glm::isnan(force_value.a))
      {
        offset.a = force_value.a;
        factor.a = 0;
      }
    }

    bool need_remapping() const
    {
      return factor!=glm::vec4(1) || offset!=glm::vec4(0) || force_value!=glm::vec4(NAN);
    }

    bool need_merging() const
    {
      return !red_channel_suffix.empty() || !green_channel_suffix.empty() || !blue_channel_suffix.empty() || !alpha_channel_suffix.empty();
    }

    bool need_processing() const
    {
      return remapSourceAsSigned || remapSourceAsSigned || need_remapping() || need_merging();
    }

    static void registerType();
  };

  TextureFile();

  void import(const QFileInfo& srcFile, ImportSettings importSettings);
  void save(const QFileInfo& textureFile);
  static GLuint loadFromFile(const QFileInfo& textureFile);

private:
  class ImportedGlTexture;
  class TextureAsFloats;

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
    quint32 rowStride, width, height, depth, mipmap;
    TextureFile::Target target;
    TextureFile::Format format;
    TextureFile::Type type;
    quint32 rawDataStart;
    quint32 rawDataLength;

    quint32 calcRowStride() const;
  };

  struct CompressedImage
  {
    quint32 rawDataStart;
    quint32 rawDataLength;
  };

  QVector<CompressedImage> compressedImages;
  QVector<UncompressedImage> uncompressedImages;
  QVector<QVector<byte>> rawData;

  quint32 appendRawData(const QVector<byte>& rawData);
  int expectedFileSize() const;

  void clear();

  static quint64 magicNumber();
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_TEXTUREFILE_H
