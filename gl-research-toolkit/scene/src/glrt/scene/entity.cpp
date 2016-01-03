#include <glrt/scene/entity.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


// ======== Entity =============================================================


Entity::Entity(Scene& scene, const Uuid<Entity>& uuid)
  : scene(scene),
    uuid(uuid)
{
}

Entity::~Entity()
{
}



// ======== Entity::ModularAttribute ===========================================


Entity::ModularAttribute::ModularAttribute(Entity& entity, const Uuid<ModularAttribute>& uuid)
  : entity(entity),
    uuid(uuid)
{
}

Entity::ModularAttribute::~ModularAttribute()
{
}


// ======== Entity::Component ==================================================


Entity::Component::Component(Entity& entity, const Uuid<Component>& uuid, bool isMovable)
  : entity(entity),
    uuid(uuid),
    isMovable(isMovable)
{
}

Entity::Component::~Component()
{
}

CoordFrame Entity::Component::globalCoordFrame() const
{
  return CoordFrame(); // FIXME::::
}




} // namespace scene
} // namespace glrt

