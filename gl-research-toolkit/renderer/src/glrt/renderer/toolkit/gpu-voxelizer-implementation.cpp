#include <glrt/renderer/toolkit/gpu-voxelizer-implementation.h>

namespace glrt {
namespace renderer {


using scene::resources::utilities::GlTexture;


struct GpuVoxelizerImplementation::VoxelizeMetaData
{
  int numVertices = 0;
  int two_sided = 0;
  float offset = 0.f;
  float factor = 1.f;
  GLuint64 vertices;
  GLuint64 targetTexture;
};


GpuVoxelizerImplementation::GpuVoxelizerImplementation()
  : metaData(sizeof(VoxelizeMetaData), gl::Buffer::UsageFlag::MAP_WRITE),
    voxelizeMeshComputeShader("distancefield-voxelizer",
                              GLRT_SHADER_DIR "/compute/voxelize.cs",
                              [](const glm::ivec3& v){return glm::ceilPowerOfTwo(v);})
{
}


GlTexture GpuVoxelizerImplementation::distanceField(const glm::ivec3& gridSize,
                                                    const scene::CoordFrame& localToVoxelSpace,
                                                    const scene::resources::StaticMesh& staticMesh,
                                                    const scene::resources::Material& material)
{
  GlTexture texture;

  GLuint textureId = texture.textureId;

  GLuint64 textureHandle = GL_RET_CALL(glGetTextureHandleNV, textureId);
  GL_CALL(glMakeTextureHandleResidentNV, textureHandle);

  texture.setUncompressed2DImage(GlTexture::TextureAsFloats::format(gridSize, 1), nullptr);

  VoxelizeMetaData& header = *reinterpret_cast<VoxelizeMetaData*>(metaData.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  header.two_sided = material.type.testFlag(scene::resources::Material::TypeFlag::TWO_SIDED);
  header.numVertices = preprocessVertices(localToVoxelSpace, staticMesh);
  header.vertices = preprocessedVertices.gpuBufferAddress();
  header.targetTexture = textureHandle;
  metaData.Unmap();

  metaData.BindUniformBuffer(0);

  voxelizeMeshComputeShader.execute(gridSize);

  GL_CALL(glMakeTextureHandleNonResidentNV, textureHandle);

  return texture;
}

int GpuVoxelizerImplementation::preprocessVertices(const scene::CoordFrame& localToVoxelSpace, const scene::resources::StaticMesh& staticMesh)
{
  const bool indexed = staticMesh.isIndexed();
  const int num_vertices = indexed ? staticMesh.indices.length() : staticMesh.vertices.length();

  int stride = int(sizeof(glm::vec3) + sizeof(float));
  GLsizeiptr necessarySpace = GLsizeiptr(num_vertices) * GLsizeiptr(stride);

  if(preprocessedVertices.GetSize() < necessarySpace)
    preprocessedVertices = std::move(gl::Buffer(necessarySpace, gl::Buffer::UsageFlag::MAP_WRITE));

  byte* buffer = reinterpret_cast<byte*>(preprocessedVertices.Map(0, necessarySpace, gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));

  if(staticMesh.isIndexed())
  {
#pragma omp parallel for
    for(int i=0; i<num_vertices; i++)
    {
      glm::vec3& vertex = *reinterpret_cast<glm::vec3*>(buffer);
      vertex = localToVoxelSpace.transform_point(staticMesh.vertices[staticMesh.indices[i]].position);
      buffer += stride;
    }
  }else
  {
#pragma omp parallel for
    for(int i=0; i<num_vertices; i++)
    {
      glm::vec3& vertex = *reinterpret_cast<glm::vec3*>(buffer);
      vertex = localToVoxelSpace.transform_point(staticMesh.vertices[i].position);
      buffer += stride;
    }
  }

  preprocessedVertices.Unmap();

  Q_ASSERT(num_vertices % 3 == 0);

  return num_vertices;
}


} // namespace renderer
} // namespace glrt
