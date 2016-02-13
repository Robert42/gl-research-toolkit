#include <glrt/scene/light-component.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/scene-layer.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


using AngelScriptIntegration::AngelScriptCheck;


LightComponent::LightComponent(Node &node, Node::Component* parent, const Uuid<LightComponent> &uuid)
  : Node::Component(node, parent, uuid)
{
  node.sceneLayer.scene.LightComponentAdded(this);
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
  : LightComponent(node, parent, uuid),
    data(data)
{
  node.sceneLayer.scene.SphereAreaLightComponentAdded(this);
}


// =============================================================================


RectAreaLightComponent::RectAreaLightComponent(Node& node, Node::Component* parent, const Uuid<RectAreaLightComponent>& uuid, const Data& data)
  : LightComponent(node, parent, uuid),
    data(data)
{
  node.sceneLayer.scene.RectAreaLightComponentAdded(this);
}



} // namespace scene
} // namespace glrt

