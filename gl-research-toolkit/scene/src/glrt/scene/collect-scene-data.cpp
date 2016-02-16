#include <glrt/scene/collect-scene-data.h>
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
  return collectData<CameraComponent,CameraParameter>(scene, [](CameraComponent* c) -> CameraParameter{c->updateGlobalCoordFrame();return c->globalCameraParameter();});
}

QHash<QString, CameraParameter> collectNamedCameraParameters(Scene* scene)
{
  return collectNamedData<CameraComponent,CameraParameter>(scene, [](CameraComponent* c) -> CameraParameter{c->updateGlobalCoordFrame();return c->globalCameraParameter();});
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
  return collectData<RectAreaLightComponent,RectAreaLightComponent::Data>(scene, [](RectAreaLightComponent* c) -> RectAreaLightComponent::Data{return c->globalCoordFrame() * c->data;});
}

QHash<QString, scene::RectAreaLightComponent::Data> collectNamedRectAreaLights(Scene* scene)
{
  return collectNamedData<RectAreaLightComponent,RectAreaLightComponent::Data>(scene, [](RectAreaLightComponent* c) -> RectAreaLightComponent::Data{return c->globalCoordFrame() * c->data;});
}


} // namespace scene
} // namespace glrt

