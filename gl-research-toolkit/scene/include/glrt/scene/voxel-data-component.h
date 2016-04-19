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

  Data data;

  VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_VOXELDATACOMPONENT_H
