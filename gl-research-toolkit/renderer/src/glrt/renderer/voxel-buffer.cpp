#include <glrt/renderer/voxel-buffer.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-data.h>
#include <glrt/scene/resources/static-mesh.h>
#include <glrt/toolkit/profiler.h>
#include <glrt/toolkit/zindex.h>

namespace glrt {
namespace renderer {

VoxelBuffer::VoxelBuffer(glrt::scene::Scene& scene)
  : scene(scene),
    voxelGridData(scene.data->voxelGrids),
    voxelBvh(scene.data->voxelBVH),
    distanceFieldVoxelData(scene.data->voxelGrids.capacity()),
    distanceFieldboundingSpheres(scene.data->voxelGrids.capacity()),
    bvhInnerBoundingSpheres(scene.data->voxelBVH.capacity()),
    bvhInnerNodes(scene.data->voxelBVH.capacity())
{
}

VoxelBuffer::~VoxelBuffer()
{
}

const VoxelBuffer::VoxelHeader& VoxelBuffer::updateVoxelHeader()
{
  PROFILE_SCOPE("VoxelBuffer::updateVoxelHeader()")

  if(Q_UNLIKELY(voxelGridData.numDynamic>0 || voxelGridData.dirtyOrder))
  {
    voxelGridData.dirtyOrder = false;

    updateVoxelGrid();
    updateBvhTree();

    _voxelHeader.numDistanceFields = voxelGridData.length;
    _voxelHeader.distanceFieldBvhInnerBoundingSpheres = this->bvhInnerBoundingSpheres.buffer.gpuBufferAddress();
    _voxelHeader.distanceFieldBvhInnerNodes = this->bvhInnerNodes.buffer.gpuBufferAddress();
    _voxelHeader.distanceFieldDataStorageBuffer = this->distanceFieldVoxelData.gpuBufferAddress();
    _voxelHeader.distanceFieldBoundingSpheres = this->distanceFieldboundingSpheres.gpuBufferAddress();
  }

  return _voxelHeader;
}

void VoxelBuffer::updateVoxelGrid()
{
  const quint16 n = voxelGridData.length;

  scene::resources::VoxelUniformDataBlock* dataBlock = distanceFieldVoxelData.Map(n);
  BoundingSphere* boundingSphere = distanceFieldboundingSpheres.Map(n);

  scene::AABB scene_aabb = scene::AABB::invalid();

  #pragma omp simd
  for(quint16 i=0; i<n; ++i)
  {
    voxelGridData.aabb_for(&scene_aabb, i);

    scene::CoordFrame globalCoordFrame = voxelGridData.globalCoordFrame(i);

    const scene::resources::VoxelData& data = voxelGridData.voxelData[i];
    dataBlock[i].globalWorldToVoxelFactor = data.localToVoxelSpace.scaleFactor / voxelGridData.scaleFactor[i];
    const glm::mat4x3 globalWorldToVoxelMatrix = data.worldToVoxelSpaceMatrix4x3(globalCoordFrame);
    const glm::ivec3 voxelCount = data.voxelCount;
    const quint64 gpuTextureHandle = data.gpuTextureHandle;

    dataBlock[i].globalWorldToVoxelMatrix_col0 = globalWorldToVoxelMatrix[0];
    dataBlock[i].globalWorldToVoxelMatrix_col1 = globalWorldToVoxelMatrix[1];
    dataBlock[i].globalWorldToVoxelMatrix_col2 = globalWorldToVoxelMatrix[2];
    dataBlock[i].globalWorldToVoxelMatrix_col3 = globalWorldToVoxelMatrix[3];
    dataBlock[i].voxelCount_x = voxelCount.x;
    dataBlock[i].voxelCount_y = voxelCount.y;
    dataBlock[i].voxelCount_z = voxelCount.z;
    dataBlock[i].texture = gpuTextureHandle;

    boundingSphere[i] = globalCoordFrame * voxelGridData.boundingSphere[i];
  }

#pragma omp simd
  for(quint16 i=0; i<n; ++i)
  {
    voxelGridData.z_index[i] = calcZIndex(scene_aabb.toUnitSpace(voxelGridData.local_coord_frame[i].position));
  }

  distanceFieldVoxelData.Unmap();
  distanceFieldboundingSpheres.Unmap();

  scene.aabb = scene_aabb;
}

void VoxelBuffer::updateBvhTree()
{
// TODO:::::::::::::::::::::::::::
#if 0
  // TODO add profiling scope
  const int numElements = distanceFieldDataStorageBuffer.numElements();
  if(numElements <= 1)
    return;
  const int numInnerNodes = numElements - 1;
  const scene::VoxelDataComponent* const * components = distanceFieldDataStorageBuffer.data();

  zIndices.resize(numElements);

  // ISSUE-61 OMP
  for(int i=0; i<numElements; ++i)
    zIndices[i] = components[i]->zIndex();

  bvhInnerBoundingSpheres.setNumElements(numInnerNodes);
  bvhInnerNodes.setNumElements(numInnerNodes);

  /* TODO uncomment
  BoundingSphere* bvhInnerBoundingSpheres = this->bvhInnerBoundingSpheres.Map();
  BVH::InnerNode* bvhInnerNodes = this->bvhInnerNodes.Map();

  BVH bvh(voxelGridData, voxelBvh);
  bvh.updateTreeCPU();

  this->bvhInnerBoundingSpheres.Unmap();
  this->bvhInnerNodes.Unmap();
  */
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
#endif
}


} // namespace renderer
} // namespace glrt
