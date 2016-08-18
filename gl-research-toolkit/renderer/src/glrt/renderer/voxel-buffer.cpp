#include <glrt/renderer/voxel-buffer.h>
#include <glrt/renderer/bvh-usage.h>
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
    distanceFieldVoxelData(scene.data->voxelGrids.capacity()),
    distanceFieldboundingSpheres(scene.data->voxelGrids.capacity()),
    bvhInnerBoundingSpheres(scene.data->voxelGrids.capacity()),
    bvhInnerNodes(scene.data->voxelGrids.capacity())
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
  PROFILE_SCOPE("VoxelBuffer::updateVoxelGrid()")

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
  PROFILE_SCOPE("VoxelBuffer::updateBvhTree()")

  const quint16 numElements = voxelGridData.length;
  if(numElements <= 1)
    return;
  const quint16 numInnerNodes = numElements - 1;

  BVH bvh(voxelGridData);

  BoundingSphere* bvhInnerBoundingSpheres = this->bvhInnerBoundingSpheres.Map(numInnerNodes);
  BVH::InnerNode* bvhInnerNodes = this->bvhInnerNodes.Map(numInnerNodes);
  bvh.updateTreeCPU(bvhInnerBoundingSpheres, bvhInnerNodes);
  this->bvhInnerBoundingSpheres.Unmap();
  this->bvhInnerNodes.Unmap();

}

BVH::BVH(const VoxelGrids& voxelGridData)
  : BVH(voxelGridData.boundingSphere, voxelGridData.z_index, voxelGridData.length)
{
}

BVH::BVH(const BoundingSphere* leaves_bounding_spheres, const quint32* leaves_z_indices, quint16 num_leaves)
  : leaves_bounding_spheres(leaves_bounding_spheres),
    leaves_z_indices(leaves_z_indices),
    num_leaves(num_leaves),
    capacity_inner_nodes(num_leaves)
{
}

void BVH::updateTreeCPU(BoundingSphere* bvhInnerBoundingSpheres, BVH::InnerNode* bvhInnerNodes)
{
  this->bvhInnerBoundingSpheres = bvhInnerBoundingSpheres;
  this->bvhInnerNodes = bvhInnerNodes;

  generateHierarchy(0, num_leaves);

  verifyHierarchy();

  this->bvhInnerBoundingSpheres = nullptr;
  this->bvhInnerNodes = nullptr;
}

void BVH::verifyHierarchy() const
{
  verifyTreeDepth();
  verifyBoundingSpheres();
}

void BVH::verifyTreeDepth() const
{
  const quint16 depth = calcDepth(0, bvhInnerNodes);
  if(depth == num_leaves-1)
    qWarning() << "Malformed tree (no performence gains)";
  qDebug() << "BVH::updateTreeCPU{num_leaves:" <<num_leaves << " depth:"<<depth<<"}";
  Q_ASSERT(depth < BVH_MAX_DEPTH);
}

void BVH::verifyBoundingSpheres(uint16_t root_node) const
{
  const uint16_t* inner_nodes = reinterpret_cast<const uint16_t*>(this->bvhInnerNodes);

  uint16_t stack[BVH_MAX_DEPTH];
  stack[0] = root_node;
  uint16_t stack_depth=uint16_t(1);

  do {
    stack_depth--;
    uint16_t current_node = stack[stack_depth];
    const BoundingSphere& currentBoundingSphere = bvhInnerBoundingSpheres[current_node];

    const uint16_t* child_nodes = inner_nodes + current_node*uint16_t(2);
    uint16_t left_node = child_nodes[0];
    uint16_t right_node = child_nodes[1];

    bool left_is_inner_node = (left_node & uint16_t(0x8000)) == uint16_t(0);
    bool right_is_inner_node = (right_node & uint16_t(0x8000)) == uint16_t(0);
    left_node = left_node & uint16_t(0x7fff);
    right_node = right_node & uint16_t(0x7fff);

    const float epsilon = 1.e-7f;

    if(left_is_inner_node)
    {
      Q_ASSERT(currentBoundingSphere.contains(bvhInnerBoundingSpheres[left_node], epsilon));
      stack[stack_depth++] = left_node;
    }else
    {
      Q_ASSERT(currentBoundingSphere.contains(leaves_bounding_spheres[left_node], epsilon));
    }

    if(right_is_inner_node)
    {
      Q_ASSERT(currentBoundingSphere.contains(bvhInnerBoundingSpheres[right_node], epsilon));
      stack[stack_depth++] = right_node;
    }else
    {
      Q_ASSERT(currentBoundingSphere.contains(leaves_bounding_spheres[right_node], epsilon));
    }

  }while(stack_depth>uint16_t(0));
}

quint16 BVH::addInnerNode()
{
  Q_ASSERT(num_inner_nodes < capacity_inner_nodes);
  return num_inner_nodes++;
}

quint16 BVH::calcDepth(quint16 root, InnerNode* bvhInnerNodes) const
{
  if(root & 0x8000)
    return 0;

  Q_ASSERT(num_inner_nodes > root);

  return 1+glm::max(calcDepth(bvhInnerNodes[root].left_child, bvhInnerNodes),
                  calcDepth(bvhInnerNodes[root].right_child, bvhInnerNodes));
}


} // namespace renderer
} // namespace glrt
