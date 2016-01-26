#include <glrt/scene/light-component.h>
#include <glrt/scene/resources/resource-manager.h>

namespace glrt {
namespace scene {


using AngelScriptIntegration::AngelScriptCheck;


LightComponent::LightComponent(Node &node, Node::Component* parent, const Uuid<LightComponent> &uuid, Interactivity interactivity)
  : Node::Component(node, parent, uuid, interactivity==Interactivity::MOVABLE),
    isStatic(interactivity==Interactivity::STATIC)
{
}


LightComponent* LightComponent::createForLightSource(Node& node,
                                                     Node::Component* parent,
                                                     const Uuid<LightComponent>& uuid,
                                                     Interactivity interactivity,
                                                     const resources::LightSource& lightSource)
{
  switch(lightSource.type)
  {
  case resources::LightSource::Type::RECT_AREA_LIGHT:
    return new RectAreaLightComponent(node, parent, uuid.cast<RectAreaLightComponent>(), lightSource.rect_area_light, interactivity);
  case resources::LightSource::Type::SPHERE_AREA_LIGHT:
    return new SphereAreaLightComponent(node, parent, uuid.cast<SphereAreaLightComponent>(), lightSource.sphere_area_light, interactivity);
  default:
    Q_UNREACHABLE();
  }
}


void LightComponent::registerAngelScriptAPIDeclarations()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  glrt::Uuid<void>::registerCustomizedUuidType("LightComponent", true);

  r = angelScriptEngine->RegisterEnum("LightSourceInteractivity"); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("LightSourceInteractivity", "STATIC", int(Interactivity::STATIC)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("LightSourceInteractivity", "DYNAMIC", int(Interactivity::DYNAMIC)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("LightSourceInteractivity", "MOVABLE", int(Interactivity::MOVABLE)); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

inline LightComponent* createLightComponent(Node& node,
                                            Node::Component* parent,
                                            const Uuid<LightComponent>& uuid,
                                            LightComponent::Interactivity interactivity,
                                            const Uuid<resources::LightSource>& lightSource)
{
  return LightComponent::createForLightSource(node,
                                              parent,
                                              uuid,
                                              interactivity,
                                              node.resourceManager().lightSourceForUuid(lightSource));
}

void LightComponent::registerAngelScriptAPI()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  Node::Component::registerCreateMethod<decltype(createLightComponent), createLightComponent>(angelScriptEngine,
                                                                                              "LightComponent",
                                                                                              "const Uuid<LightComponent> &in uuid, LightSourceInteractivity interactivity, const Uuid<LightSource> &in lightSourceUuid");

  Node::Component::registerAsBaseOfClass<LightComponent>(angelScriptEngine, "LightComponent");

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


// =============================================================================


SphereAreaLightComponent::SphereAreaLightComponent(Node& node, Node::Component* parent, const Uuid<SphereAreaLightComponent>& uuid, const Data& data, Interactivity interactivity)
  : LightComponent(node, parent, uuid, interactivity),
    data(data)
{
}


// =============================================================================


RectAreaLightComponent::RectAreaLightComponent(Node& node, Node::Component* parent, const Uuid<RectAreaLightComponent>& uuid, const Data& data, Interactivity interactivity)
  : LightComponent(node, parent, uuid, interactivity),
    data(data)
{
}



} // namespace scene
} // namespace glrt

