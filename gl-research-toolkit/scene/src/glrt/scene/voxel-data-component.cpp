#include <glrt/scene/voxel-data-component.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


VoxelDataComponent::VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid, const Data& data)
  : Component(node, parent, uuid),
    data(data)
{
  scene().VoxelDataComponentAdded(this);
}


VoxelDataComponent::Data VoxelDataComponent::globalData() const
{
  return globalCoordFrame() * data;
}


} // namespace scene
} // namespace glrt
