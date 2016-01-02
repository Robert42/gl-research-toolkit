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


Entity::ModularAttribute::ModularAttribute(const Uuid<ModularAttribute>& uuid)
  : Object(uuid)
{
}


// ======== Entity::SpatialComponent ==================================================


Entity::Component::Component(const Uuid<Component>& uuid, bool isMovable)
  : Object(uuid),
    isMovable(isMovable)
{
}




} // namespace scene
} // namespace glrt

