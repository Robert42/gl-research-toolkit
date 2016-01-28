#include <glrt/scene/collect-scene-data.h>

namespace glrt {
namespace scene {

Array<Node::TickingObject*> collectAllTickingObjects(Scene* scene)
{
  Array<Node::TickingObject*> tickingObjects;
  int totalNumberTickingObjects = 0;
  for(const SceneLayer* layer : scene->allLayers())
    totalNumberTickingObjects += layer->allNodes().length();

  tickingObjects.reserve(totalNumberTickingObjects);

  for(const SceneLayer* layer : scene->allLayers())
  {
    for(Node* n : layer->allNodes())
    {
      for(Node::Component* component : n->allComponents())
        if(component->tickTraits() != Node::TickingObject::TickTraits::NoTick)
          tickingObjects.append(component);
      for(Node::ModularAttribute* attribute : n->allModularAttributes())
        if(attribute->tickTraits() != Node::TickingObject::TickTraits::NoTick)
          tickingObjects.append(attribute);
    }
  }

  return std::move(tickingObjects);
}

QVector<Camera> collectCameras(Scene* scene)
{
  return collectData<CameraComponent,Camera>(scene, [](CameraComponent* c) -> Camera{return c->globalCoordFrame() * c->cameraParameter;});
}

QHash<QString, Camera> collectNamedCameras(Scene* scene)
{
  return collectNamedData<CameraComponent,Camera>(scene, [](CameraComponent* c) -> Camera{return c->globalCoordFrame() * c->cameraParameter;});
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

