#include <glrt/scene/entity.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


// ======== Entity =============================================================


Entity::Entity(Scene& scene)
  : scene(scene)
{
  setParent(&scene);
}


QVector<Entity::Component*> Entity::components()
{
  return _components;
}


// ======== Entity::Component ==================================================


Entity::Component::Component(Entity& entity)
  : entity(entity)
{
  setParent(&entity);
}


} // namespace scene
} // namespace glrt

