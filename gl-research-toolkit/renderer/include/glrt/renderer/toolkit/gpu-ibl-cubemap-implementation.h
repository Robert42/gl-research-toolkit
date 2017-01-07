#ifndef GLRT_RENDERER_GPUIBLCUBEMAPS_H
#define GLRT_RENDERER_GPUIBLCUBEMAPS_H

#include <glrt/scene/resources/utilities/calculate_ibl_cubemaps.h>
#include <glrt/renderer/toolkit/compute-shader-set.h>
#include <glhelper/buffer.hpp>

namespace glrt {
namespace renderer {

using scene::resources::TextureFile;
using scene::resources::utilities::GlTexture;

class GpuIblCubemapImplementation final : public TextureFile::IblCalculator::Implementation
{
public:
  GpuIblCubemapImplementation();

  void execute(TextureFile::IblCalculator* calculator, const TextureFile::GlTexture& texture, GlTexture::Target target_texture, int layer, int level, const glm::mat4& side_rotation) final override;

private:
  struct Header
  {
    GLuint64 target;
    GLuint64 source;
    glm::mat4 rotation;
  };

  ComputeShaderSet shader;
  gl::Buffer header_buffer;

  static QSet<QString> preprocessorFromType(TextureFile::IblCalculator::Type type);
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_GPUIBLCUBEMAPS_H
