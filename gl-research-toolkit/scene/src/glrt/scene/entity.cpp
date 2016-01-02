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


void Entity::addModularAttribute(ModularAttribute* modularAttribute)
{
  modularAttribute->set_entity(this->as_ref<Entity>());
}

void Entity::removeModularAttribute(ModularAttribute* modularAttribute)
{
  Q_ASSERT(modularAttribute->get_entity().lock().ptr() == this);
  modularAttribute->set_entity(ref<Entity>());
}


// ======== Entity::Component ================================================


Entity::ModularAttribute::ModularAttribute(const Uuid<ModularAttribute>& uuid)
  : Object(uuid)
{
}

void Entity::ModularAttribute::set_entity(const ref<Entity>& entity)
{
  this->entity = entity;
}

weakref<Entity> Entity::ModularAttribute::get_entity()
{
  return entity;
}


// ======== Entity::SpatialComponent ==================================================


Entity::Component::Component(const Uuid<Component>& uuid, bool isMovable)
  : Object(uuid),
    isMovable(isMovable)
{
}




} // namespace scene
} // namespace glrt

