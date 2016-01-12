#ifndef GLRT_SCENE_LIGHTCOMPONENT_H
#define GLRT_SCENE_LIGHTCOMPONENT_H

#include <glrt/scene/node.h>
#include <glrt/toolkit/geometry.h>
#include <glrt/scene/resources/light-source.h>

namespace glrt {
namespace scene {


class LightComponent : public Node::Component
{
public:
  typedef resources::LightSource::Interactivity Interactivity;
  const bool isStatic : 1;

  LightComponent(Node& entity, const Uuid<LightComponent>& uuid, Interactivity interactivity);
};


class SphereAreaLightComponent : public LightComponent
{
public:
  typedef resources::LightSource::SphereAreaLight Data;
  Data data;

  SphereAreaLightComponent(Node& entity, const Uuid<SphereAreaLightComponent>& uuid, const Data& data, Interactivity interactivity);
};


class RectAreaLightComponent : public LightComponent
{
public:
  typedef resources::LightSource::RectAreaLight Data;
  Data data;

  RectAreaLightComponent(Node& entity, const Uuid<RectAreaLightComponent>& uuid, const Data& data, resources::LightSource::Interactivity interactivity);
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_LIGHTCOMPONENT_H
