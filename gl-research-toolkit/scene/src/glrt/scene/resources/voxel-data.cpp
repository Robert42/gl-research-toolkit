#include <glrt/scene/resources/voxel-data.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/texture-manager.h>

namespace glrt {
namespace scene {
namespace resources {


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
