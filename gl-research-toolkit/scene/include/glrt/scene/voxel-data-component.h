#ifndef GLRT_SCENE_VOXELDATACOMPONENT_H
#define GLRT_SCENE_VOXELDATACOMPONENT_H

#include <glrt/scene/node.h>
#include <glrt/scene/resources/voxel-data.h>

namespace glrt {
namespace scene {

class VoxelDataComponent : public glrt::scene::Node::Component
{
public:
  typedef resources::VoxelData Data;

  struct AABB
  {
    glm::mat4 worldToVoxelSpace = glm::mat4(1);
    glm::ivec3 voxelCount = glm::ivec3(1);
    padding<int, 1> _padding;
  };

  Data data;

  VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid, const Data& data=Data());

  AABB globalAabbData() const;
  quint64 textureData() const;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_VOXELDATACOMPONENT_H
