#include <glrt/scene/resources/voxel-data.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/texture-manager.h>
#include <glrt/scene/voxel-data-component.h>

namespace glrt {
namespace scene {
namespace resources {


VoxelBoundingBox::VoxelBoundingBox(const VoxelDataComponent& c)
{
  worldToVoxelSpace = c.globalWorldToVoxelMatrix4();
  voxelCount = c.data.voxelCount;
}


CoordFrame VoxelData::worldToVoxelSpaceCoordFrame(const CoordFrame& localToWorldSpace) const
{
  return localToVoxelSpace * localToWorldSpace.inverse();
}

glm::mat4x3 VoxelData::worldToVoxelSpaceMatrix4x3(const CoordFrame& localToWorldSpace) const
{
  return worldToVoxelSpaceCoordFrame(localToWorldSpace).toMat4x3();
}

glm::mat4 VoxelData::worldToVoxelSpaceMatrix4(const CoordFrame& localToWorldSpace) const
{
  return worldToVoxelSpaceCoordFrame(localToWorldSpace).toMat4();
}

BoundingSphere VoxelData::worldSpaceBoundignSphere(const CoordFrame& localToWorldSpace) const
{
  return localToWorldSpace * this->boundingSphere;
}

VoxelData VoxelIndex::toData(ResourceManager& resourceManager) const
{
  VoxelData data;

  data.gpuTextureHandle = resourceManager.textureManager.gpuHandle(resourceManager.textureManager.handleFor(this->texture3D));
  data.voxelCount = this->gridSize;
  data.localToVoxelSpace = this->localToVoxelSpace;
  data.boundingSphere = this->boundingSphere;

  return data;
}


} // namespace resources
} // namespace scene
} // namespace glrt
