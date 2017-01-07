#include <glrt/renderer/toolkit/gpu-ibl-cubemap-implementation.h>
#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace renderer {

GpuIblCubemapImplementation::GpuIblCubemapImplementation()
  : shader("calculate_ibl", GLRT_SHADER_DIR "/compute/ibl.cs", [](const glm::ivec3& v) -> glm::ivec3{return v;}),
    header_buffer(sizeof(GLuint64), gl::Buffer::UsageFlag::MAP_WRITE)
{

}

void GpuIblCubemapImplementation::execute(TextureFile::IblCalculator* calculator, const TextureFile::GlTexture& sourceTexture, GlTexture::Target target, int layer, int level, const glm::mat4& side_rotation)
{
  Q_UNUSED(target);

  GlTexture& target_texture = calculator->target_textures[layer];

  QSet<QString> proprocessorBlock = preprocessorFromType(calculator->type);

  GLuint sourceTextureId = sourceTexture.textureId;
  GLuint targetTextureId = target_texture.textureId;

  GLuint64 targetTextureHandle = GL_RET_CALL(glGetImageHandleNV, targetTextureId, level, GL_TRUE, 0, GL_RGBA16F);
  GL_CALL(glMakeImageHandleResidentNV, targetTextureHandle, GL_WRITE_ONLY);

  GLuint64 sourceTextureHandle = GL_RET_CALL(glGetTextureHandleNV, sourceTextureId);
  GL_CALL(glMakeTextureHandleResidentNV, sourceTextureHandle);

  Header& header = *reinterpret_cast<Header*>(header_buffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  header.target = targetTextureHandle;
  header.source = sourceTextureHandle;
  header.rotation = side_rotation;
  header.roughness = level / glm::log2<float>(target_texture.width(0));
  header.sampleCount = 8192;
  header_buffer.Unmap();

  header_buffer.BindUniformBuffer(IBL_BLOCK);

  Q_ASSERT(target_texture.width(level) == (calculator->size >> level));
  Q_ASSERT(target_texture.height(level) == (calculator->size >> level));

  shader.execute(glm::ivec3(target_texture.width(level),
                            target_texture.height(level),
                            1),
                 proprocessorBlock);

  GL_CALL(glMakeImageHandleNonResidentNV, targetTextureHandle);
  GL_CALL(glMakeTextureHandleNonResidentNV, sourceTextureHandle);
}

QSet<QString> GpuIblCubemapImplementation::preprocessorFromType(TextureFile::IblCalculator::Type type)
{
  return {QString("#define GGX %0").arg(int(type==TextureFile::IblCalculator::Type::GGX)),
          QString("#define DIFFUSE %0").arg(int(type==TextureFile::IblCalculator::Type::DIFFUSE)),
          QString("#define CONE_60 %0").arg(int(type==TextureFile::IblCalculator::Type::CONE_60)),
          QString("#define CONE_45 %0n").arg(int(type==TextureFile::IblCalculator::Type::CONE_45)),
          QString("#define MAX_NUM_GRID_CASCADES 1"),
          QString("#define NUM_GRID_CASCADES 1"),
          QString("#define SDF_CANDIDATE_GRID_SIZE 4")};
}

} // namespace renderer
} // namespace glrt
