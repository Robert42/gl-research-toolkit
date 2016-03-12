#ifndef GLRT_SCENE_RESOURCES_TEXTUREFILE_H
#define GLRT_SCENE_RESOURCES_TEXTUREFILE_H

#include <glrt/dependencies.h>
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
    enum class Format
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
    enum class Type
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

    Type type = Type::UINT8;
    Format format = Format::BGRA;
    int width = -1;
    int height = -1;

    // #TODO test, whether the sampler code in the shader accepts signed bytes:
    int sourceIsNormalMap = false;

    static void registerType();
  };

  TextureFile();

  void import(QFileInfo& srcFile, const ImportSettings& importSettings);
  void save(QFileInfo& textureFile);
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_TEXTUREFILE_H
