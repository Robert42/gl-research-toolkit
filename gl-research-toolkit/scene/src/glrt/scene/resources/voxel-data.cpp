#include <glrt/scene/resources/voxel-data.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/texture-manager.h>

namespace glrt {
namespace scene {
namespace resources {


glm::mat4 VoxelData::worldToVoxelSpaceMatrix(const CoordFrame& localToWorldSpace) const
{
  glm::mat4 meshScaleFactor(1.f);

  meshScaleFactor[0][0] = this->meshScaleFactor[0];
  meshScaleFactor[1][1] = this->meshScaleFactor[1];
  meshScaleFactor[2][2] = this->meshScaleFactor[2];

  return meshScaleFactor * (localToVoxelSpace * localToWorldSpace.inverse()).toMat4();
}


VoxelData VoxelIndex::toData(ResourceManager& resourceManager) const
{
  VoxelData data;

  data.gpuTextureHandle = resourceManager.textureManager.gpuHandle(resourceManager.textureManager.handleFor(this->texture3D));
  data.voxelCount = this->gridSize;
  data.localToVoxelSpace = this->localToVoxelSpace;
  data.meshScaleFactor = this->meshScaleFactor;

  return data;
}


} // namespace resources
} // namespace scene
} // namespace glrt
