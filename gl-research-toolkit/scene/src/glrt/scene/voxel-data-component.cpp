#include <glrt/scene/voxel-data-component.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-data.h>

namespace glrt {
namespace scene {


using resources::BoundingSphere;


VoxelDataComponent::VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid, const Data& data, const BoundingSphere& boundingSphere, bool voxelizedAsScenery)
  : Component(node, parent, uuid, DataClass::VOXELGRID)
{
  Scene::Data::VoxelGrids* voxelGrids = scene().data->voxelGrids;
  voxelGrids->voxelData[data_index.array_index] = data;
  voxelGrids->boundingSphere[data_index.array_index] = boundingSphere;
  voxelGrids->voxelizedAsScenery[data_index.array_index] = voxelizedAsScenery;

  scene().VoxelDataComponentAdded(this);
}

VoxelDataComponent::~VoxelDataComponent()
{
  Scene::Data::VoxelGrids* voxelGrids = scene().data->voxelGrids;
  voxelGrids->swap_voxel_data(data_index.array_index, voxelGrids->last_item_index());
}

glm::mat4x3 VoxelDataComponent::globalWorldToVoxelMatrix4x3() const
{
  Scene::Data::VoxelGrids* voxelGrids = scene().data->voxelGrids;
  const quint16 i = data_index.array_index;
  glm::mat4x3 m = voxelGrids->voxelData[i].worldToVoxelSpaceMatrix4x3(voxelGrids->globalCoordFrame(i));
  return m;
}

glm::mat4 VoxelDataComponent::globalWorldToVoxelMatrix4() const
{
  Scene::Data::VoxelGrids* voxelGrids = scene().data->voxelGrids;
  const quint16 i = data_index.array_index;
  glm::mat4 m = voxelGrids->voxelData[i].worldToVoxelSpaceMatrix4(voxelGrids->globalCoordFrame(i));
  return m;
}

glm::ivec3 VoxelDataComponent::gridSize() const
{
  return data().voxelCount;
}

quint64 VoxelDataComponent::textureData() const
{
  return data().gpuTextureHandle;
}

BoundingSphere VoxelDataComponent::boundingSphere() const
{
  Scene::Data::VoxelGrids* voxelGrids = scene().data->voxelGrids;
  const quint16 i = data_index.array_index;
  return voxelGrids->globalCoordFrame(i) * voxelGrids->boundingSphere[i];
}

const VoxelDataComponent::Data&VoxelDataComponent::data() const
{
  Scene::Data::VoxelGrids* voxelGrids = scene().data->voxelGrids;
  return voxelGrids->voxelData[data_index.array_index];
}

bool VoxelDataComponent::voxelizedAsScenery() const
{
  Scene::Data::VoxelGrids* voxelGrids = scene().data->voxelGrids;
  return voxelGrids->voxelizedAsScenery[data_index.array_index];
}

AABB VoxelDataComponent::globalAABB() const
{
  Scene::Data::VoxelGrids* voxelGrids = scene().data->voxelGrids;
  const quint16 i = data_index.array_index;

  AABB aabb = AABB::invalid();
  voxelGrids->aabb_for(&aabb, i);
  return aabb;
}


} // namespace scene
} // namespace glrt
