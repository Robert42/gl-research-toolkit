#include <glrt/renderer/voxel-buffer.h>

namespace glrt {
namespace renderer {

VoxelBuffer::VoxelBuffer(glrt::scene::Scene& scene)
  : distanceFieldMat4StorageBuffer(scene),
    distanceFieldGridSizesStorageBuffer(scene),
    distanceFieldSpaceFactorStorageBuffer(scene),
    distanceFieldTextureHandleStorageBuffer(scene)
{
}

VoxelBuffer::~VoxelBuffer()
{
}

quint32 VoxelBuffer::numVisibleVoxelGrids() const
{
  Q_ASSERT(distanceFieldGridSizesStorageBuffer.numElements() == distanceFieldMat4StorageBuffer.numElements());
  Q_ASSERT(distanceFieldGridSizesStorageBuffer.numElements() == distanceFieldSpaceFactorStorageBuffer.numElements());
  Q_ASSERT(distanceFieldGridSizesStorageBuffer.numElements() == distanceFieldTextureHandleStorageBuffer.numElements());

  return static_cast<quint32>(distanceFieldGridSizesStorageBuffer.numElements());
}

const VoxelBuffer::VoxelHeader& VoxelBuffer::updateVoxelHeader()
{
  distanceFieldMat4StorageBuffer.update();
  distanceFieldGridSizesStorageBuffer.update();
  distanceFieldSpaceFactorStorageBuffer.update();
  distanceFieldTextureHandleStorageBuffer.update();

  _voxelHeader.numDistanceFields = numVisibleVoxelGrids();
  _voxelHeader.distanceFieldWorldToVoxelMatrices = distanceFieldMat4StorageBuffer.gpuBufferAddress();
  _voxelHeader.distanceFieldGridSizes = distanceFieldGridSizesStorageBuffer.gpuBufferAddress();
  _voxelHeader.distanceFieldSpaceFactors = distanceFieldSpaceFactorStorageBuffer.gpuBufferAddress();
  _voxelHeader.distanceFieldTextures = distanceFieldTextureHandleStorageBuffer.gpuBufferAddress();

  return _voxelHeader;
}

} // namespace renderer
} // namespace glrt
