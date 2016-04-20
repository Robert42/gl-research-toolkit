#ifndef GLRT_SCENE_RESOURCES_VOXELDATA_H
#define GLRT_SCENE_RESOURCES_VOXELDATA_H

#include <glrt/dependencies.h>
#include <glrt/scene/coord-frame.h>

namespace glrt {
namespace scene {
namespace resources {


struct VoxelData
{
  glm::mat4 worldToVoxelSpace = glm::mat4(1);
  glm::ivec3 voxelCount = glm::ivec3(1);
  padding<int, 1> _padding;
  quint64 gpuTextureHandle = 0;

  friend VoxelData operator*(const CoordFrame& localToWorldSpace, VoxelData data)
  {
    data.worldToVoxelSpace = data.worldToVoxelSpace * localToWorldSpace.inverse().toMat4();
    return data;
  }
};


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_VOXELDATA_H
