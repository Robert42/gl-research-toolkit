#include <glrt/renderer/toolkit/gpu-voxelizer-implementation.h>
#include <glrt/system.h>
#include <QThread>
#include <QElapsedTimer>

namespace glrt {
namespace renderer {


using scene::resources::utilities::GlTexture;


struct GpuVoxelizerImplementation::VoxelizeMetaData
{
  int numVertices = 0;
  int indexOffset = 0;
  padding<int, 2> _padding1;
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
                                                    const scene::resources::TriangleArray& staticMesh,
                                                    MeshType meshType)
{
  int totalNumVoxels = gridSize.x*gridSize.y*gridSize.z;

  GlTexture texture;
  texture.setUncompressed2DImage(GlTexture::TextureAsFloats::format(gridSize, 1), nullptr);
  texture.makeComplete();

  GLuint textureId = texture.textureId;

  GLuint64 imageHandle = GL_RET_CALL(glGetImageHandleNV, textureId, 0, GL_TRUE, 0, GL_R32F);
  GL_CALL(glMakeImageHandleResidentNV, imageHandle, GL_WRITE_ONLY);

  int numVertices = preprocessVertices(localToVoxelSpace, staticMesh);

  QElapsedTimer timer;
  timer.start();

  QSet<QString> proprocessorBlock;
  if(meshType == MeshType::MANIFOLD_RAY_CHECK)
    proprocessorBlock.insert("#define MANIFOLD_RAY_CHECK");
  else if(meshType == MeshType::TWO_SIDED)
    proprocessorBlock.insert("#define TWO_SIDED");
  else if(meshType == MeshType::FACE_SIDE)
    proprocessorBlock.insert("#define FACE_SIDE");
  else
    Q_UNREACHABLE();

  int dispatchSize = glm::min(totalNumVoxels, 64*64*64);
  for(int i=0; i<totalNumVoxels; i+=dispatchSize)
  {
    if(timer.elapsed() > 1000)
      qDebug() << "  Voxelization-progress:" << 100.*double(i) / double(totalNumVoxels) << "%";
    VoxelizeMetaData& header = *reinterpret_cast<VoxelizeMetaData*>(metaData.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
    header.numVertices = numVertices;
    header.vertices = preprocessedVertices.gpuBufferAddress();
    header.targetTexture = imageHandle;
    header.indexOffset = i;
    metaData.Unmap();

    metaData.BindUniformBuffer(0);

    voxelizeMeshComputeShader.execute(glm::ivec3(dispatchSize, 1, 1), proprocessorBlock);
    QThread::msleep(1);
  }

  GL_CALL(glMakeImageHandleNonResidentNV, imageHandle);

  return texture;
}

int GpuVoxelizerImplementation::preprocessVertices(const scene::CoordFrame& localToVoxelSpace, const scene::resources::TriangleArray& staticMesh)
{
  if(staticMesh.vertices.size() > std::numeric_limits<int>::max())
    throw GLRT_EXCEPTION(QString("Mesh is too large to voxelize with %0 vertices").arg(staticMesh.vertices.size()));

  const int num_vertices = quint32(staticMesh.vertices.size());

  int stride = quint32(sizeof(glm::vec3) + sizeof(float));
  GLsizeiptr necessarySpace = GLsizeiptr(num_vertices) * GLsizeiptr(stride);

  if(preprocessedVertices.GetSize() < necessarySpace)
    preprocessedVertices = std::move(gl::Buffer(necessarySpace, gl::Buffer::UsageFlag::MAP_WRITE));

  byte* const buffer = reinterpret_cast<byte*>(preprocessedVertices.Map(0, necessarySpace, gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));


#pragma omp parallel for
  for(int i=0; i<num_vertices; i++)
  {
    glm::vec3& vertex = *reinterpret_cast<glm::vec3*>(buffer + i*stride);
    vertex = localToVoxelSpace.transform_point(staticMesh[i]);
  }

  preprocessedVertices.Unmap();

  Q_ASSERT(num_vertices % 3 == 0);

  return num_vertices;
}


} // namespace renderer
} // namespace glrt
