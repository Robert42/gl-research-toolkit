#ifndef GLRT_SCENE_ENTITY_H
#define GLRT_SCENE_ENTITY_H

#include <glrt/dependencies.h>

namespace glrt {
namespace scene {

class Scene;
class Entity final : public QObject
{
  Q_OBJECT
public:
  class Component;

  glm::mat4 relativeTransform = glm::mat4(1);
  Scene& scene;

  Entity(Scene& scene);
  ~Entity();

  QVector<Entity::Component*> components();

  template<typename T>
  QVector<T*> allComponentsWithType(const std::function<bool(T*)>& filter);

  glm::mat4 globalTransformation() const;

private:
  QVector<Entity::Component*> _components;
};


class Entity::Component : public QObject
{
  Q_OBJECT
public:
  Entity& entity;

  Component(Entity& entity);
};


class VisibleComponent : public Entity::Component
{
public:
  glm::mat4 relativeTransform = glm::mat4(1);
  bool movable : 1;

  VisibleComponent(Entity& entity);

  glm::mat4 globalTransformation() const;
};


template<typename T>
QVector<T*> Entity::allComponentsWithType(const std::function<bool(T*)>& filter)
{
  static_assert(std::is_base_of<Entity::Component, T>::value, "T must inherit from Entity::Component");

  QVector<T*> components;
  components.reserve((_components.size()+3) / 4);

  for(Component* c : _components)
  {
    T* component = qobject_cast<T*>(c);
    if(component && filter(component))
      components.append(component);
  }
  return components;
}


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_ENTITY_H
