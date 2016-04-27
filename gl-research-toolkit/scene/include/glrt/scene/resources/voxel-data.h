#ifndef GLRT_SCENE_RESOURCES_VOXELDATA_H
#define GLRT_SCENE_RESOURCES_VOXELDATA_H

#include <glrt/dependencies.h>
#include <glrt/scene/coord-frame.h>
#include <glrt/scene/resources/declarations.h>
#include <glrt/scene/resources/voxelizer.h>

namespace glrt {
namespace scene {
namespace resources {


struct VoxelData
{
  CoordFrame localToVoxelSpace;
  glm::vec3 meshScaleFactor = glm::vec3(1);
  glm::ivec3 voxelCount = glm::ivec3(1);
  quint64 gpuTextureHandle = 0;

  glm::mat4 worldToVoxelSpaceMatrix(const CoordFrame& localToWorldSpace) const;
};

struct VoxelIndex
{
  CoordFrame localToVoxelSpace;
  glm::vec3 meshScaleFactor = glm::vec3(1);
  glm::ivec3 gridSize = glm::ivec3(0);
  Uuid<Texture> texture3D;
  float factor = 1.f; // reserved for future usage (in case uint8 is used for signed distacne fields -- currently unused and must be 1)
  float offset = 0.f; // reserved for future usage (in case uint8 is used for signed distacne fields -- currently unused and must be 0)

  VoxelData toData(ResourceManager& resourceManager) const;
};


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_VOXELDATA_H
