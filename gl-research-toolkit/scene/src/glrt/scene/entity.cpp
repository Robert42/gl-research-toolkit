#include <glrt/scene/entity.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


// ======== Entity =============================================================


Entity::Entity(const Uuid<Entity>& uuid)
  : Object(uuid)
{
}


Entity::~Entity()
{
}


// ======== Entity::Component ================================================


Entity::Component::Component(const Uuid<Component>& uuid)
  : Object(uuid)
{
}


// ======== Entity::SpatialComponent ==================================================


Entity::SpatialComponent::SpatialComponent(const Uuid<SpatialComponent>& uuid, bool isMovable)
  : Component(uuid),
    isMovable(isMovable)
{
}




} // namespace scene
} // namespace glrt

