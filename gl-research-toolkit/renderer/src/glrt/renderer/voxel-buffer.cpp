#include <glrt/renderer/voxel-buffer.h>

namespace glrt {
namespace renderer {

VoxelBuffer::VoxelBuffer(glrt::scene::Scene& scene)
  : distanceFieldAABBsStorageBuffer(scene),
    distanceFieldTextureHandleStorageBuffer(scene)
{
}

VoxelBuffer::~VoxelBuffer()
{
}

quint32 VoxelBuffer::numVisibleVoxelGrids() const
{
  Q_ASSERT(distanceFieldAABBsStorageBuffer.numElements() == distanceFieldTextureHandleStorageBuffer.numElements());

  return static_cast<quint32>(distanceFieldAABBsStorageBuffer.numElements());
}

const VoxelBuffer::VoxelHeader& VoxelBuffer::updateVoxelHeader()
{
  distanceFieldAABBsStorageBuffer.update();
  distanceFieldTextureHandleStorageBuffer.update();

  _voxelHeader.numDistanceFields = numVisibleVoxelGrids();
  _voxelHeader.distanceFieldAABBs = distanceFieldAABBsStorageBuffer.gpuBufferAddress();
  _voxelHeader.distanceFieldTextures = distanceFieldTextureHandleStorageBuffer.gpuBufferAddress();

  return _voxelHeader;
}

} // namespace renderer
} // namespace glrt
