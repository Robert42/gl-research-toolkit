#include <glrt/scene/light-component.h>

namespace glrt {
namespace scene {


using AngelScriptIntegration::AngelScriptCheck;


LightComponent::LightComponent(Node &entity, const Uuid<LightComponent> &uuid, Interactivity interactivity)
  : Node::Component(entity, uuid, interactivity==Interactivity::MOVABLE),
    isStatic(interactivity==Interactivity::STATIC)
{
}


// =============================================================================


SphereAreaLightComponent::SphereAreaLightComponent(Node& entity, const Uuid<SphereAreaLightComponent>& uuid, const Data& data, Interactivity interactivity)
  : LightComponent(entity, uuid, interactivity),
    data(data)
{
}


// =============================================================================


RectAreaLightComponent::RectAreaLightComponent(Node& entity, const Uuid<RectAreaLightComponent>& uuid, const Data& data, Interactivity interactivity)
  : LightComponent(entity, uuid, interactivity),
    data(data)
{
}



} // namespace scene
} // namespace glrt

