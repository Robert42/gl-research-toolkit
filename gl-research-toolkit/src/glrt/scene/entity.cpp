#include <glrt/scene/entity.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


// ======== Entity =============================================================


Entity::Entity(Scene& scene)
  : scene(scene)
{
  setParent(&scene);
  scene.AddEntity(this);
}


Entity::~Entity()
{
  QVector<Entity::Component*> components;

  components.swap(this->_components);

  for(Entity::Component* c : components)
    delete c;

  scene.RemoveEntity(this);
}


QVector<Entity::Component*> Entity::components()
{
  return _components;
}


glm::mat4 Entity::globalTransformation() const
{
  return this->relativeTransform;
}


// ======== Entity::Component ==================================================


Entity::Component::Component(Entity& entity)
  : entity(entity)
{
  setParent(&entity);
  entity._components.append(this);
}


// ======== Entity::Component ==================================================


VisibleComponent::VisibleComponent(Entity& entity, bool movable, const glm::mat4& relativeTransform)
  : Component(entity),
    movable(movable),
    relativeTransform(relativeTransform)
{
}


glm::mat4 VisibleComponent::globalTransformation() const
{
  return entity.globalTransformation() * this->relativeTransform;
}


} // namespace scene
} // namespace glrt

