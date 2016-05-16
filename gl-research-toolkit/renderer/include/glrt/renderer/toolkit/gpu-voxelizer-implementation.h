#ifndef GLRT_RENDERER_GPUVOXELIZERIMPLEMENTATION_H
#define GLRT_RENDERER_GPUVOXELIZERIMPLEMENTATION_H

#include <glrt/renderer/dependencies.h>
#include <glrt/renderer/toolkit/compute-shader-set.h>
#include <glrt/scene/resources/voxelizer.h>
#include <glhelper/buffer.hpp>

namespace glrt {
namespace renderer {

class GpuVoxelizerImplementation final : public scene::resources::Voxelizer::Implementation
{
public:
  struct VoxelizeMetaData;

  GpuVoxelizerImplementation();

  scene::resources::utilities::GlTexture distanceField(const glm::ivec3& gridSize,
                                                       const scene::CoordFrame& localToVoxelSpace,
                                                       const scene::resources::TriangleArray<>& staticMesh,
                                                       MeshType meshType) override;

private:
  gl::Buffer metaData;
  gl::Buffer preprocessedVertices;
  ComputeShaderSet voxelizeMeshComputeShader;

  int preprocessVertices(const scene::CoordFrame& localToVoxelSpace, const scene::resources::TriangleArray<>& staticMesh);
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_GPUVOXELIZERIMPLEMENTATION_H
