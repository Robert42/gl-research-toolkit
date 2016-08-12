#include <glrt/scene/voxel-data-component.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-data.h>

namespace glrt {
namespace scene {


using resources::BoundingSphere;


VoxelDataComponent::VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid, const Data& data, bool voxelizedAsScenery)
  : Component(node, parent, uuid, DataClass::VOXELGRID)
{
  scene().VoxelDataComponentAdded(this);

  Scene::Data::VoxelGrids& voxelGrids = scene().data->voxelGrids;
  voxelGrids.voxelData[data_index.array_index] = data;
  voxelGrids.voxelizedAsScenery[data_index.array_index] = voxelizedAsScenery;

  // TODO::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: delete
  PRINT_VALUE(data.localToVoxelSpace);
  PRINT_VALUE(data.localToVoxelSpace.transform_point(glm::vec3(-1,  1, -1)));
  PRINT_VALUE(data.localToVoxelSpace.transform_point(glm::vec3( 1, -1, 1)));
}

VoxelDataComponent::~VoxelDataComponent()
{
  Scene::Data::VoxelGrids& voxelGrids = scene().data->voxelGrids;
  voxelGrids.swap_voxel_data(data_index.array_index, voxelGrids.last_item_index());
}

glm::mat4x3 VoxelDataComponent::globalWorldToVoxelMatrix4x3() const
{
  glm::mat4x3 m = data().worldToVoxelSpaceMatrix4x3(globalCoordFrame());
  return m;
}

glm::mat4 VoxelDataComponent::globalWorldToVoxelMatrix4() const
{
  glm::mat4 m = data().worldToVoxelSpaceMatrix4(globalCoordFrame());
  return m;
}

glm::ivec3 VoxelDataComponent::gridSize() const
{
  return data().voxelCount;
}


// TODO:::::::::::::::::::::::::::::::::
#if 0
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
#endif


quint64 VoxelDataComponent::textureData() const
{
  return data().gpuTextureHandle;
}

BoundingSphere VoxelDataComponent::boundingSphere() const
{
  return data().worldSpaceBoundignSphere(globalCoordFrame());
}

const VoxelDataComponent::Data&VoxelDataComponent::data() const
{
  Scene::Data::VoxelGrids& voxelGrids = scene().data->voxelGrids;
  return voxelGrids.voxelData[data_index.array_index];
}

bool VoxelDataComponent::voxelizedAsScenery() const
{
  Scene::Data::VoxelGrids& voxelGrids = scene().data->voxelGrids;
  return voxelGrids.voxelizedAsScenery[data_index.array_index];
}


} // namespace scene
} // namespace glrt
