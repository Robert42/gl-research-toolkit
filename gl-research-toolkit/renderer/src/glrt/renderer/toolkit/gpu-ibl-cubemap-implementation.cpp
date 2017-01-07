#include <glrt/renderer/toolkit/gpu-ibl-cubemap-implementation.h>

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

  QSet<QString> proprocessorBlock = preprocessorFromType(calculator->type);

  GLuint sourceTextureId = sourceTexture.textureId;
  GLuint targetTextureId = calculator->target_textures[layer].textureId;

  GLuint64 targetTextureHandle = GL_RET_CALL(glGetImageHandleNV, targetTextureId, level, GL_TRUE, 0, GL_RGBA16F);
  GL_CALL(glMakeImageHandleResidentNV, targetTextureHandle, GL_WRITE_ONLY);

  GLuint64 sourceTextureHandle = GL_RET_CALL(glGetTextureHandleNV, sourceTextureId);
  GL_CALL(glMakeTextureHandleResidentNV, sourceTextureHandle);

  Header& header = *reinterpret_cast<Header*>(header_buffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  header.source = sourceTextureHandle;
  header.target = targetTextureHandle;
  header.rotation = side_rotation;
  header_buffer.Unmap();

  shader.execute(glm::ivec3(glm::ivec2(calculator->size >> level), 1), proprocessorBlock);

  GL_CALL(glMakeImageHandleNonResidentNV, targetTextureHandle);
  GL_CALL(glMakeTextureHandleNonResidentNV, targetTextureHandle);
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
