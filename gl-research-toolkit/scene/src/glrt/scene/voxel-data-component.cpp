#include <glrt/scene/voxel-data-component.h>

namespace glrt {
namespace scene {


VoxelDataComponent::VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid, const Data& data)
  : Component(node, parent, uuid),
    data(data)
{
}

} // namespace scene
} // namespace glrt
