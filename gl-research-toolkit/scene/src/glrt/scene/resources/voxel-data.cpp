#include <glrt/scene/resources/voxel-data.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/texture-manager.h>

namespace glrt {
namespace scene {
namespace resources {


VoxelData VoxelIndex::toData(ResourceManager& resourceManager) const
{
  VoxelData data;

  data.gpuTextureHandle = resourceManager.textureManager.gpuHandle(resourceManager.textureManager.handleFor(this->texture3D));
  data.voxelCount = this->gridSize;
  data.worldToVoxelSpace = this->localToVoxelSpace.toMat4();

  return data;
}


} // namespace resources
} // namespace scene
} // namespace glrt
