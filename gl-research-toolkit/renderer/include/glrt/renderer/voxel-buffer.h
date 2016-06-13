#ifndef GLRT_RENDERER_VOXELBUFFER_H
#define GLRT_RENDERER_VOXELBUFFER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/renderer/simple-shader-storage-buffer.h>


namespace glrt {
namespace renderer {

class VoxelBuffer
{
public:
  struct VoxelHeader
  {
    GLuint64 distanceFieldBoundingSpheres;
    GLuint64 distanceFieldDataStorageBuffer;
    //padding<GLuint64, 1> _padding1;
    quint32 numDistanceFields;
    padding<quint32, 3> _padding2;
  };

  VoxelBuffer(scene::Scene& scene);
  ~VoxelBuffer();

  const VoxelHeader& updateVoxelHeader();

private:
  SimpleShaderStorageBuffer<scene::VoxelDataComponent, implementation::RandomComponentDataDescription<scene::VoxelDataComponent, scene::VoxelDataComponent::VoxelDataBlock, &scene::VoxelDataComponent::voxelDataBlock>> distanceFieldDataStorageBuffer;
  SimpleShaderStorageBuffer<scene::VoxelDataComponent, implementation::RandomComponentDataDescription<scene::VoxelDataComponent, scene::resources::BoundingSphere, &scene::VoxelDataComponent::boundingSphere>> distanceFieldBoundingSphereStorageBuffer;

  VoxelHeader _voxelHeader;

  quint32 numVisibleVoxelGrids() const;

  quint32 _numVisibleVoxelGrids = 0;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_VOXELBUFFER_H
