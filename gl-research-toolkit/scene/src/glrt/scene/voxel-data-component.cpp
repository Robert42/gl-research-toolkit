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


VoxelDataComponent::AABB VoxelDataComponent::globalAabbData() const
{
  AABB aabb;

  aabb.worldToVoxelSpace = (data.localToVoxelSpace * globalCoordFrame().inverse()).toMat4();
  aabb.voxelCount = data.voxelCount;

  return aabb;
}

quint64 VoxelDataComponent::textureData() const
{
  return data.gpuTextureHandle;
}


} // namespace scene
} // namespace glrt
