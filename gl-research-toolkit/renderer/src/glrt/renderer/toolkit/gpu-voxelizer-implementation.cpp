#include <glrt/renderer/toolkit/gpu-voxelizer-implementation.h>

namespace glrt {
namespace renderer {


using scene::resources::utilities::GlTexture;


struct GpuVoxelizerImplementation::VoxelizeMetaData
{
  int numVertices = 0;
  int two_sided = 0;
  int indexOffset;
  padding<int, 1> _padding1;
  GLuint64 targetTexture;
  padding<GLuint64, 1> _padding2;
  GLuint64 vertices;
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
                                                    MeshType meshType)
{
  int totalNumVoxels = gridSize.x*gridSize.y*gridSize.z;

  GlTexture texture;
  texture.setUncompressed2DImage(GlTexture::TextureAsFloats::format(gridSize, 1), nullptr);

  // Make the texture complete
  GL_CALL(glTextureParameteri, texture.textureId, GL_TEXTURE_BASE_LEVEL, 0);
  GL_CALL(glTextureParameteri, texture.textureId, GL_TEXTURE_MAX_LEVEL, 0);

  GLuint textureId = texture.textureId;

  GLuint64 imageHandle = GL_RET_CALL(glGetImageHandleNV, textureId, 0, GL_TRUE, 0, GL_R32F);
  GL_CALL(glMakeImageHandleResidentNV, imageHandle, GL_WRITE_ONLY);

  int numVertices = preprocessVertices(localToVoxelSpace, staticMesh);

  VoxelizeMetaData& header = *reinterpret_cast<VoxelizeMetaData*>(metaData.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  header.two_sided = meshType == MeshType::TWO_SIDED;
  header.numVertices = numVertices;
  header.vertices = preprocessedVertices.gpuBufferAddress();
  header.targetTexture = imageHandle;
  header.indexOffset = 0;
  metaData.Unmap();

  metaData.BindUniformBuffer(0);

  voxelizeMeshComputeShader.execute(glm::ivec3(totalNumVoxels, 1, 1));

  GL_CALL(glMakeImageHandleNonResidentNV, imageHandle);

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

  byte* const buffer = reinterpret_cast<byte*>(preprocessedVertices.Map(0, necessarySpace, gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));

  if(staticMesh.isIndexed())
  {
#pragma omp parallel for
    for(int i=0; i<num_vertices; i++)
    {
      glm::vec3& vertex = *reinterpret_cast<glm::vec3*>(buffer + i*stride);
      vertex = localToVoxelSpace.transform_point(staticMesh.vertices[staticMesh.indices[i]].position);
    }
  }else
  {
#pragma omp parallel for
    for(int i=0; i<num_vertices; i++)
    {
      glm::vec3& vertex = *reinterpret_cast<glm::vec3*>(buffer + i*stride);
      vertex = localToVoxelSpace.transform_point(staticMesh.vertices[i].position);
    }
  }

  preprocessedVertices.Unmap();

  Q_ASSERT(num_vertices % 3 == 0);

  return num_vertices;
}


} // namespace renderer
} // namespace glrt
