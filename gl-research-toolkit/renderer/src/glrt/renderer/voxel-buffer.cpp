#include <glrt/renderer/voxel-buffer.h>

namespace glrt {
namespace renderer {

VoxelBuffer::VoxelBuffer(glrt::scene::Scene& scene)
  : distanceFieldDataStorageBuffer(scene),
    distanceFieldBoundingSphereStorageBuffer(scene)
{
}

VoxelBuffer::~VoxelBuffer()
{
}

quint32 VoxelBuffer::numVisibleVoxelGrids() const
{
  Q_ASSERT(distanceFieldDataStorageBuffer.numElements() == distanceFieldBoundingSphereStorageBuffer.numElements());

  return static_cast<quint32>(distanceFieldDataStorageBuffer.numElements());
}

void VoxelBuffer::updateBvhTree()
{
  // #TODO add profiling scope

  const int numElements = distanceFieldDataStorageBuffer.numElements();
  bvhInnerBoundingSpheres.setNumElements(numElements);
  bvhInnerNodes.setNumElements(numElements);

  BoundingSphere* bvhInnerBoundingSpheres = this->bvhInnerBoundingSpheres.Map();
  InnerNode* bvhInnerNodes = this->bvhInnerNodes.Map();


  this->bvhInnerBoundingSpheres.Unmap();
  this->bvhInnerNodes.Unmap();
}

const VoxelBuffer::VoxelHeader& VoxelBuffer::updateVoxelHeader()
{
  distanceFieldDataStorageBuffer.update();
  distanceFieldBoundingSphereStorageBuffer.update();

  updateBvhTree();

  _voxelHeader.numDistanceFields = numVisibleVoxelGrids();
  _voxelHeader.distanceFieldDataStorageBuffer = distanceFieldDataStorageBuffer.gpuBufferAddress();
  _voxelHeader.distanceFieldBoundingSpheres = distanceFieldBoundingSphereStorageBuffer.gpuBufferAddress();

  return _voxelHeader;
}

} // namespace renderer
} // namespace glrt
