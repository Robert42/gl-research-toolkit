#include <glrt/renderer/voxelized-scene.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/voxel-data-component.h>

namespace glrt {
namespace renderer {

using scene::StaticMeshComponent;
using scene::VoxelDataComponent;
using scene::resources::StaticMesh;
using scene::resources::VoxelIndex;
using scene::resources::VoxelData;
using scene::resources::Voxelizer;
using scene::resources::ResourceIndex;
using scene::resources::ResourceManager;

VoxelizedScene::VoxelizedScene(glrt::scene::Scene& scene)
  : scene(scene)
{
}

void VoxelizedScene::enabledSignedDistanceFields()
{
  connect(&scene, &glrt::scene::Scene::StaticMeshComponentAdded, this, &VoxelizedScene::appendSignedDistanceField);
}

void VoxelizedScene::appendSignedDistanceField(StaticMeshComponent* staticMeshComponent)
{
  Uuid<StaticMesh> meshUuid = staticMeshComponent->staticMeshUuid;

  ResourceManager& resourceManager = scene.resourceManager;
  const ResourceIndex* index = resourceManager.indexForResourceUuid(meshUuid, nullptr);
  if(index == nullptr)
    return;

  Uuid<VoxelIndex> voxelIndexUuid = index->voxelIndicesIndex.value(meshUuid).value(Voxelizer::FieldType::SIGNED_DISTANCE_FIELD, Uuid<VoxelIndex>());

  if(voxelIndexUuid.toQUuid().isNull())
    return;

  const VoxelIndex& voxelIndex = index->voxelIndices.value(voxelIndexUuid);
  VoxelData voxelGpuData = voxelIndex.toData(resourceManager);

  new VoxelDataComponent(staticMeshComponent->node, staticMeshComponent, Uuid<VoxelDataComponent>::create(), voxelGpuData);
}

} // namespace renderer
} // namespace glrt
