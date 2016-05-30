#include <glrt/scene/voxel-data-component.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


using resources::BoundingSphere;


VoxelDataComponent::VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid, const Data& data, bool voxelizedAsScenery)
  : Component(node, parent, uuid),
    data(data),
    voxelizedAsScenery(voxelizedAsScenery)
{
  scene().VoxelDataComponentAdded(this);
}


glm::mat4 VoxelDataComponent::globalWorldToVoxelMatrix() const
{
  return data.worldToVoxelSpaceMatrix(globalCoordFrame());
}

VoxelDataComponent::GridSize VoxelDataComponent::gridSize() const
{
  GridSize gridSize;

  gridSize.voxelCount = data.voxelCount;

  return gridSize;
}

VoxelDataComponent::WorldVoxelUvwSpaceFactor VoxelDataComponent::spaceFactor() const
{
  WorldVoxelUvwSpaceFactor factor;

  factor.voxelToUvwSpace = 1.f / glm::vec3(data.voxelCount);
  factor.voxelToWorldSpace = globalCoordFrame().scaleFactor / data.localToVoxelSpace.scaleFactor;

  return factor;
}

quint64 VoxelDataComponent::textureData() const
{
  return data.gpuTextureHandle;
}

BoundingSphere VoxelDataComponent::boundingSphere() const
{
  return data.worldSpaceBoundignSphere(globalCoordFrame());
}


} // namespace scene
} // namespace glrt
