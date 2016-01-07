#include <glrt/scene/light-component.h>

namespace glrt {
namespace scene {


using AngelScriptIntegration::AngelScriptCheck;


LightComponent::LightComponent(Entity &entity, const Uuid<LightComponent> &uuid, Interactivity interactivity)
  : Entity::Component(entity, uuid, interactivity==Interactivity::MOVABLE),
    isStatic(interactivity==Interactivity::STATIC)
{
}


// =============================================================================


SphereAreaLightComponent::SphereAreaLightComponent(Entity& entity, const Uuid<SphereAreaLightComponent>& uuid, const Data& data, Interactivity interactivity)
  : LightComponent(entity, uuid, interactivity),
    data(data)
{
}


// =============================================================================


RectAreaLightComponent::RectAreaLightComponent(Entity& entity, const Uuid<RectAreaLightComponent>& uuid, const Data& data, Interactivity interactivity)
  : LightComponent(entity, uuid, interactivity),
    data(data)
{
}



} // namespace scene
} // namespace glrt

