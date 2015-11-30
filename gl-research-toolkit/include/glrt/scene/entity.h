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

  glm::mat4 absoluteTransform = glm::mat4(1);
  Scene& scene;

  Entity(Scene& scene);
  ~Entity();

  QVector<Entity::Component*> components();

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

  VisibleComponent(Entity& entity);
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_ENTITY_H
