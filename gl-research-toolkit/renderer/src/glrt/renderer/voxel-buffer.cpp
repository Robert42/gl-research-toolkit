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

void VoxelBuffer::updateBvhTree()
{
  // #TODO add profiling scope
  const int numElements = distanceFieldDataStorageBuffer.numElements();
  const int numInnerNodes = numElements - 1;
  const scene::VoxelDataComponent* const * components = distanceFieldDataStorageBuffer.data();

  zIndices.resize(numElements);

  // ISSUE-61 OMP
  for(int i=0; i<numElements; ++i)
    zIndices[i] = components[i]->zIndex();

  bvhInnerBoundingSpheres.setNumElements(numInnerNodes);
  bvhInnerNodes.setNumElements(numInnerNodes);

  BoundingSphere* bvhInnerBoundingSpheres = this->bvhInnerBoundingSpheres.Map();
  BVH::InnerNode* bvhInnerNodes = this->bvhInnerNodes.Map();

  BVH bvh(components, bvhInnerBoundingSpheres, bvhInnerNodes, zIndices.data(), numElements, numInnerNodes);
  bvh.updateTreeCPU();

  this->bvhInnerBoundingSpheres.Unmap();
  this->bvhInnerNodes.Unmap();
}

BVH::BVH(const scene::VoxelDataComponent* const * leaveBoundingSpheres, BoundingSphere* bvhInnerBoundingSpheres, InnerNode* bvhInnerNodes, const quint32* zIndices, int length, int innerNodesCapacity)
  : leaves(leaveBoundingSpheres),
    bvhInnerBoundingSpheres(bvhInnerBoundingSpheres),
    bvhInnerNodes(bvhInnerNodes),
    zIndices(zIndices),
    numLeaves(length),
    innerNodesCapacity(innerNodesCapacity),
    numInnerNodes(0)
{
}

void BVH::updateTreeCPU()
{
  generateHierarchy(0, numLeaves);
}

int BVH::addInnerNode()
{
  Q_ASSERT(numInnerNodes < innerNodesCapacity);
  return numInnerNodes++;
}


} // namespace renderer
} // namespace glrt
