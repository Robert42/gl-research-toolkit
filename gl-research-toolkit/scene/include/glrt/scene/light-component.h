#ifndef GLRT_SCENE_LIGHTCOMPONENT_H
#define GLRT_SCENE_LIGHTCOMPONENT_H

#include <glrt/scene/node.h>
#include <glrt/toolkit/geometry.h>
#include <glrt/scene/resources/light-source.h>

namespace glrt {
namespace scene {


class LightComponent : public Node::Component
{
  Q_OBJECT
public:
  LightComponent(Node& node, Node::Component* parent, const Uuid<LightComponent>& uuid, DataClass dataClass);
  ~LightComponent();

  static LightComponent* createForLightSource(Node& node, Node::Component* parent, const Uuid<LightComponent>& uuid, const resources::LightSource& lightSource);

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();
};


class SphereAreaLightComponent : public LightComponent
{
public:
  typedef resources::LightSource::SphereAreaLight Data;
  Data data;

  SphereAreaLightComponent(Node& node, Node::Component* parent, const Uuid<SphereAreaLightComponent>& uuid, const Data& data);
  ~SphereAreaLightComponent();

  void setGlobalData(const Data& data) const;
  Data globalData() const;
};


class RectAreaLightComponent : public LightComponent
{
public:
  typedef resources::LightSource::RectAreaLight Data;

  RectAreaLightComponent(Node& node, Node::Component* parent, const Uuid<RectAreaLightComponent>& uuid, const Data& data);
  ~RectAreaLightComponent();

  void setGlobalData(const Data& data) const;
  Data globalData() const;
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_LIGHTCOMPONENT_H
