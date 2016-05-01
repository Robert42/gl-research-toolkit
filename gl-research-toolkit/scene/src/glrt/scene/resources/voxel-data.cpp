#include <glrt/scene/resources/voxel-data.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/texture-manager.h>
#include <glrt/scene/voxel-data-component.h>

namespace glrt {
namespace scene {
namespace resources {


VoxelBoundingBox::VoxelBoundingBox(const VoxelDataComponent& c)
{
  worldToVoxelSpace = c.globalWorldToVoxelMatrix();
  voxelCount = c.data.voxelCount;
}


glm::mat4 VoxelData::worldToVoxelSpaceMatrix(const CoordFrame& localToWorldSpace) const
{
  return (localToVoxelSpace * localToWorldSpace.inverse()).toMat4();
}


VoxelData VoxelIndex::toData(ResourceManager& resourceManager) const
{
  VoxelData data;

  data.gpuTextureHandle = resourceManager.textureManager.gpuHandle(resourceManager.textureManager.handleFor(this->texture3D));
  data.voxelCount = this->gridSize;
  data.localToVoxelSpace = this->localToVoxelSpace;

  return data;
}


} // namespace resources
} // namespace scene
} // namespace glrt
