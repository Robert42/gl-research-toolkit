#ifndef GLRT_SCENE_RESOURCES_UTILITIES_GLTEXTURE_H
#define GLRT_SCENE_RESOURCES_UTILITIES_GLTEXTURE_H

#include <glrt/dependencies.h>
#include <glhelper/texture2d.hpp>
#include <glhelper/texture3d.hpp>
#include <QObject>

namespace glrt {
namespace scene {
namespace resources {
namespace utilities {

class GlTexture : public QObject
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
    RED_INTEGER  = GL_RED_INTEGER,
    RG_INTEGER = GL_RG_INTEGER,
    RGB_INTEGER = GL_RGB_INTEGER,
    RGBA_INTEGER = GL_RGBA_INTEGER,
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

  struct TextureAsFloats;

  static int channelsPerPixelForFormat(Format format);
  static int bytesPerPixelForType(Type type);
  static int bytesPerPixelForFormatType(Format format, Type type);
  static GLenum internalFormat(Format format, Type type, bool* supported=nullptr);

  struct ImportSettings
  {
    bool generateMipmaps = true;
    bool scaleDownToPowerOfTwo = true;
  };


  struct UncompressedImage
  {
    quint32 alignment, rowStride, width, height, depth, mipmap;
    Target target;
    Format format;
    Type type;
    quint32 rawDataStart;
    quint32 rawDataLength;

    quint32 calcRowStride() const;
  };

  struct CompressedImage
  {
    quint32 rawDataStart;
    quint32 rawDataLength;
  };

  GLuint textureId;

  GlTexture();
  GlTexture(const QFileInfo& fileToBeImported, ImportSettings importSettings);
  GlTexture(GlTexture&& other);
  void operator=(GlTexture&& other);

  GlTexture(const GlTexture&) = delete;
  GlTexture& operator=(const GlTexture&) = delete;

  ~GlTexture();

  GLint width(int level) const;
  GLint height(int level) const;
  GLint depth(int level) const;

  GLint maxLevel() const;
  Target target() const;

  static UncompressedImage format(const glm::uvec3& size, int level, Format format, Type type, Target target);

  QPair<UncompressedImage, QVector<byte>> uncompressed2DImage(int level,
                                                              Format format,
                                                              Type type) const;
  void setUncompressed2DImage(const GlTexture::UncompressedImage& image, const void* data);
  void makeComplete();
  TextureAsFloats asFloats(int level);
  void fromFloats(const TextureAsFloats& texture);
};


struct GlTexture::TextureAsFloats
{
  UncompressedImage image;
  QVector<float> textureData;
  byte* data = nullptr;
  quint32 width = 0, components_per_row=0, height = 0, depth = 0, rowCount=0;

  static UncompressedImage format(quint32 width, quint32 height, quint32 depth=1, quint32 numComponents=4);
  static UncompressedImage format(const glm::ivec3& textureSize, quint32 numComponents=4);

  TextureAsFloats() = delete;

  TextureAsFloats(quint32 width, quint32 height, quint32 depth=1, quint32 numComponents=4);
  TextureAsFloats(const glm::ivec3& size, quint32 numComponents);

  TextureAsFloats(const QImage& qImage);

  TextureAsFloats(const QPair<UncompressedImage, QVector<byte>>& importedTexture);

  void remapSourceAsSigned();
  void remapSourceAsUnsigned();
  void remap(glm::vec4 offset, glm::vec4 factor);

  void mergeWith_as_grey(const TextureAsFloats* red, const TextureAsFloats* green, const TextureAsFloats* blue, const TextureAsFloats* alpha);
  void mergeWith_channelwise(const TextureAsFloats* red, const TextureAsFloats* green, const TextureAsFloats* blue, const TextureAsFloats* alpha);
  void mergeWith(const TextureAsFloats* red, const TextureAsFloats* green, const TextureAsFloats* blue, const TextureAsFloats* alpha,
                 bool merge_red_as_grey, bool merge_green_as_grey, bool merge_blue_as_grey, bool merge_alpha_as_grey);

  void flipY();

  void fromQImage(QImage image);
  QImage asQImage() const;
  QImage asChannelQImage(int channel) const;

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

  inline static float grey(const glm::vec3& x)
  {
    return (x.r + x.g + x.b) / 3.f;
  }
};



} // namespace utilities
} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_UTILITIES_GLTEXTURE_H
