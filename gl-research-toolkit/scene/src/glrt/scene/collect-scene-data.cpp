#include <glrt/scene/collect-scene-data.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/fps-debug-controller.h>

namespace glrt {
namespace scene {

CameraComponent* findDebugCameraComponent(Scene* scene)
{
  for(CameraComponent* cameraComponent : collectAllComponentsWithType<CameraComponent>(scene))
  {
    if(cameraComponent->uuid == uuids::debugCameraComponent)
      return cameraComponent;
  }

  return nullptr;
}

FpsDebugController* findFpsDebugController(Scene* scene)
{
  for(FpsDebugController* fps : collectAllModularAttributesWithType<FpsDebugController>(scene))
  {
    if(fps != nullptr)
      return fps;
  }

  return nullptr;
}

QVector<CameraParameter> collectCameraParameters(Scene* scene)
{
  return collectData<CameraComponent,CameraParameter>(scene, [](CameraComponent* c) -> CameraParameter{/*TODO remove or uncomment c->updateGlobalCoordFrame();*/return c->globalCameraParameter();});
}

QHash<QString, CameraParameter> collectNamedCameraParameters(Scene* scene)
{
  return collectNamedData<CameraComponent,CameraParameter>(scene, [](CameraComponent* c) -> CameraParameter{/*TODO remove or uncomment c->updateGlobalCoordFrame();*/return c->globalCameraParameter();});
}


QVector<SphereAreaLightComponent::Data> collectSphereAreaLights(Scene* scene)
{
  return collectData<SphereAreaLightComponent,SphereAreaLightComponent::Data>(scene, [](SphereAreaLightComponent* c) -> SphereAreaLightComponent::Data{return c->globalCoordFrame() * c->data;});
}

QHash<QString, SphereAreaLightComponent::Data> collectNamedSphereAreaLights(Scene* scene)
{
  return collectNamedData<SphereAreaLightComponent,SphereAreaLightComponent::Data>(scene, [](SphereAreaLightComponent* c) -> SphereAreaLightComponent::Data{return c->globalCoordFrame() * c->data;});
}


QVector<RectAreaLightComponent::Data> collectRectAreaLights(Scene* scene)
{
  return collectData<RectAreaLightComponent,RectAreaLightComponent::Data>(scene, [](RectAreaLightComponent* c) -> RectAreaLightComponent::Data{return c->globalData();});
}

QHash<QString, scene::RectAreaLightComponent::Data> collectNamedRectAreaLights(Scene* scene)
{
  return collectNamedData<RectAreaLightComponent,RectAreaLightComponent::Data>(scene, [](RectAreaLightComponent* c) -> RectAreaLightComponent::Data{return c->globalData();});
}


QVector<VoxelBoundingBox> collectVoxelGridSize(Scene* scene)
{
  return collectData<VoxelDataComponent,VoxelBoundingBox>(scene, [](VoxelDataComponent* c) -> VoxelBoundingBox{return VoxelBoundingBox(*c);});
}

QHash<QString, VoxelBoundingBox> collectNamedVoxelGridSize(Scene* scene)
{
  return collectNamedData<VoxelDataComponent,VoxelBoundingBox>(scene, [](VoxelDataComponent* c) -> VoxelBoundingBox{return VoxelBoundingBox(*c);});
}

QVector<BoundingSphere> collectVoxelBoundingSphere(Scene* scene)
{
  return collectData<VoxelDataComponent,BoundingSphere>(scene, [](VoxelDataComponent* c) -> BoundingSphere{return c->boundingSphere();});
}

QHash<QString, BoundingSphere> collectNamedVoxelBoundingSphere(Scene* scene)
{
  return collectNamedData<VoxelDataComponent,BoundingSphere>(scene, [](VoxelDataComponent* c) -> BoundingSphere{return c->boundingSphere();});
}

QVector<AABB> collectBoundingBoxes(Scene* scene)
{
  return collectData<StaticMeshComponent,AABB>(scene, [](StaticMeshComponent* c) -> AABB{return c->globalAABB();});
}

QHash<QString, AABB> collectNamedBoundingBoxes(Scene* scene)
{
  return collectNamedData<StaticMeshComponent,AABB>(scene, [](StaticMeshComponent* c) -> AABB{return c->globalAABB();});
}


} // namespace scene
} // namespace glrt

