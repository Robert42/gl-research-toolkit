#ifndef GLRT_SCENE_VOXELDATACOMPONENT_H
#define GLRT_SCENE_VOXELDATACOMPONENT_H

#include <glrt/scene/node.h>

namespace glrt {
namespace scene {

class VoxelDataComponent : public glrt::scene::Node::Component
{
public:
  struct Data
  {
    quint64 gpuTextureHandle = 0;
    glm::mat4 worldToVoxelSpace = glm::mat4(1);
    glm::ivec3 voxelCount = glm::ivec3(1);
  };

  Data data;

  VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_VOXELDATACOMPONENT_H
