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


glm::mat4x3 VoxelDataComponent::globalWorldToVoxelMatrix4x3() const
{
  glm::mat4x3 m = data.worldToVoxelSpaceMatrix4x3(globalCoordFrame());
  return m;
}

glm::mat4 VoxelDataComponent::globalWorldToVoxelMatrix4() const
{
  glm::mat4 m = data.worldToVoxelSpaceMatrix4(globalCoordFrame());
  return m;
}

glm::ivec3 VoxelDataComponent::gridSize() const
{
  return data.voxelCount;
}

VoxelDataComponent::VoxelDataBlock VoxelDataComponent::voxelDataBlock() const
{
  VoxelDataBlock dataBlock;

  glm::mat4x3 globalWorldToVoxelMatrix = this->globalWorldToVoxelMatrix4x3();

  dataBlock.globalWorldToVoxelFactor = data.localToVoxelSpace.scaleFactor / globalCoordFrame().scaleFactor;
  dataBlock.globalWorldToVoxelMatrix_col0 = globalWorldToVoxelMatrix[0];
  dataBlock.globalWorldToVoxelMatrix_col1 = globalWorldToVoxelMatrix[1];
  dataBlock.globalWorldToVoxelMatrix_col2 = globalWorldToVoxelMatrix[2];
  dataBlock.globalWorldToVoxelMatrix_col3 = globalWorldToVoxelMatrix[3];
  dataBlock.voxelCount_x = data.voxelCount.x;
  dataBlock.voxelCount_y = data.voxelCount.y;
  dataBlock.voxelCount_z = data.voxelCount.z;
  dataBlock.texture = textureData();

  return dataBlock;
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
