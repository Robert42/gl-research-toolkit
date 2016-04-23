#include <glrt/renderer/voxel-buffer.h>

namespace glrt {
namespace renderer {

VoxelBuffer::VoxelBuffer(glrt::scene::Scene& scene)
  : voxelDataStorageBuffer(scene)
{
}

VoxelBuffer::~VoxelBuffer()
{
}

const VoxelBuffer::VoxelHeader& VoxelBuffer::updateVoxelHeader()
{
  voxelDataStorageBuffer.update();

  _voxelHeader.numDistanceFields = numVisibleVoxelGrids();
  _voxelHeader.distanceFieldDataHeaders = voxelDataStorageBuffer.gpuBufferAddress();

  return _voxelHeader;
}

} // namespace renderer
} // namespace glrt
