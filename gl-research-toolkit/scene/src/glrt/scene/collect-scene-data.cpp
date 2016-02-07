#include <glrt/scene/collect-scene-data.h>

namespace glrt {
namespace scene {


QVector<CameraParameter> collectCameras(Scene* scene)
{
  return collectData<CameraComponent,CameraParameter>(scene, [](CameraComponent* c) -> CameraParameter{return c->updateGlobalCoordFrame() * c->cameraParameter;});
}

QHash<QString, CameraParameter> collectNamedCameras(Scene* scene)
{
  return collectNamedData<CameraComponent,CameraParameter>(scene, [](CameraComponent* c) -> CameraParameter{return c->updateGlobalCoordFrame() * c->cameraParameter;});
}

// #TODO: use the new Array type?
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

