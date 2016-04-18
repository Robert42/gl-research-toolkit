#ifndef GLRT_SCENE_VOXELDATACOMPONENT_H
#define GLRT_SCENE_VOXELDATACOMPONENT_H

#include <glrt/scene/node.h>

namespace glrt {
namespace scene {


struct VoxelData
{
  quint64 gpuTextureHandle = 0;
  glm::mat4 worldToVoxelSpace = glm::mat4(1);
  glm::ivec3 voxelCount = glm::ivec3(1);

  friend VoxelData operator*(const CoordFrame& coordFrame, VoxelData data)
  {
    data.worldToVoxelSpace = data.worldToVoxelSpace * coordFrame.inverse().toMat4();
    return data;
  }
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
