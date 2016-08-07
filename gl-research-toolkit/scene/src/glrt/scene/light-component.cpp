#include <glrt/scene/light-component.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/scene-layer.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-data.h>

namespace glrt {
namespace scene {


using AngelScriptIntegration::AngelScriptCheck;


LightComponent::LightComponent(Node &node, Node::Component* parent, const Uuid<LightComponent> &uuid, DataClass dataClass)
  : Node::Component(node, parent, uuid, dataClass)
{
  scene().LightComponentAdded(this);
}

LightComponent::~LightComponent()
{
  hideInDestructor();
}


LightComponent* LightComponent::createForLightSource(Node& node,
                                                     Node::Component* parent,
                                                     const Uuid<LightComponent>& uuid,
                                                     const resources::LightSource& lightSource)
{
  switch(lightSource.type)
  {
  case resources::LightSource::Type::RECT_AREA_LIGHT:
    return new RectAreaLightComponent(node, parent, uuid.cast<RectAreaLightComponent>(), lightSource.rect_area_light);
  case resources::LightSource::Type::SPHERE_AREA_LIGHT:
    return new SphereAreaLightComponent(node, parent, uuid.cast<SphereAreaLightComponent>(), lightSource.sphere_area_light);
  default:
    Q_UNREACHABLE();
  }
}


void LightComponent::registerAngelScriptAPIDeclarations()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  glrt::Uuid<void>::registerCustomizedUuidType("LightComponent", true);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

inline LightComponent* createLightComponent(Node& node,
                                            Node::Component* parent,
                                            const Uuid<LightComponent>& uuid,
                                            const Uuid<resources::LightSource>& lightSource)
{
  return LightComponent::createForLightSource(node,
                                              parent,
                                              uuid,
                                              node.resourceManager().lightSourceForUuid(lightSource));
}

void LightComponent::registerAngelScriptAPI()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  Node::Component::registerCreateMethod<decltype(createLightComponent), createLightComponent>(angelScriptEngine,
                                                                                              "LightComponent",
                                                                                              "const Uuid<LightComponent> &in uuid, const Uuid<LightSource> &in lightSourceUuid");

  Node::Component::registerAsBaseOfClass<LightComponent>(angelScriptEngine, "LightComponent");

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

// =============================================================================


SphereAreaLightComponent::SphereAreaLightComponent(Node& node, Node::Component* parent, const Uuid<SphereAreaLightComponent>& uuid, const Data& data)
  : LightComponent(node, parent, uuid, DataClass::SPHERELIGHT)
{
  setGlobalData(data);

  scene().SphereAreaLightComponentAdded(this);
}

SphereAreaLightComponent::~SphereAreaLightComponent()
{
  hideInDestructor();
}

void SphereAreaLightComponent::setGlobalData(const SphereAreaLightComponent::Data& data) const
{
  quint16 index = data_index.array_index;
  Scene::Data::SphereLights& sphereLights = scene().data->sphereLights;

  sphereLights.radius[index] = data.radius;
  sphereLights.lightData[index] = data.areaLightCommon;
  sphereLights.local_coord_frame[index] = CoordFrame(data.areaLightCommon.origin);
}

SphereAreaLightComponent::Data SphereAreaLightComponent::globalData() const
{
  quint16 index = data_index.array_index;
  Scene::Data::SphereLights& sphereLights = scene().data->sphereLights;

  SphereAreaLightComponent::Data data;

  data.radius = sphereLights.globalRadius(index);
  data.areaLightCommon = sphereLights.lightData[index];
  data.areaLightCommon.origin = sphereLights.position[index];

  return data;
}


// =============================================================================


RectAreaLightComponent::RectAreaLightComponent(Node& node, Node::Component* parent, const Uuid<RectAreaLightComponent>& uuid, const Data& data)
  : LightComponent(node, parent, uuid, DataClass::RECTLIGHT)
{
  setGlobalData(data);

  scene().RectAreaLightComponentAdded(this);
}

RectAreaLightComponent::~RectAreaLightComponent()
{
  hideInDestructor();
}

void RectAreaLightComponent::setGlobalData(const RectAreaLightComponent::Data& data) const
{
  quint16 index = data_index.array_index;
  Scene::Data::RectLights& rectLights = scene().data->rectLights;

  rectLights.lightData[index] = data.areaLightCommon;
  rectLights.half_size[index] = glm::vec2(data.half_width, data.half_height);
  rectLights.local_coord_frame[index] = CoordFrame(glm::mat4(glm::vec4(data.tangent1, 0.f),
                                                             glm::vec4(data.tangent2, 0.f),
                                                             glm::vec4(data.normal(), 0.f),
                                                             glm::vec4(data.areaLightCommon.origin, 1.f)));
}

RectAreaLightComponent::Data RectAreaLightComponent::globalData() const
{
  quint16 index = data_index.array_index;
  Scene::Data::RectLights& rectLights = scene().data->rectLights;

  RectAreaLightComponent::Data data;

  const glm::vec2 half_size = rectLights.globalHalfSize(index);

  data.half_width = half_size.x;
  data.half_height = half_size.y;
  data.areaLightCommon = rectLights.lightData[index];
  data.areaLightCommon.origin = rectLights.position[index];
  rectLights.globalTangents(index, &data.tangent1, &data.tangent2);

  return data;
}



} // namespace scene
} // namespace glrt

