#ifndef GLRT_SCENE_VOXELDATACOMPONENT_H
#define GLRT_SCENE_VOXELDATACOMPONENT_H

#include <glrt/scene/node.h>

namespace glrt {
namespace scene {


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

  void initSize(const glm::vec3& meshBoundingBoxMin, const glm::vec3& meshBoundingBoxMax, float extend, int maxSize, float voxelsPerMeter);
};

class VoxelDataComponent : public glrt::scene::Node::Component
{
public:
  typedef VoxelData Data;

  Data data;

  VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_VOXELDATACOMPONENT_H
