#include <glrt/scene/voxel-data-component.h>

namespace glrt {
namespace scene {


VoxelDataComponent::VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid)
  : Component(node, parent, uuid)
{
}


} // namespace scene
} // namespace glrt
