#ifndef GLRT_SCENE_ENTITY_H
#define GLRT_SCENE_ENTITY_H

#include <glrt/dependencies.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/object.h>
#include <glrt/scene/coord-frame.h>

namespace glrt {
namespace scene {


class Scene;
class Entity : public scene::Object
{
public:
  class Component;
  class SpatialComponent;

  template<typename T>
  QVector<T*> allLogicModuleWithType(const std::function<bool(T*)>& filter=always_return_true) const;
  template<typename T>
  QVector<T*> allComponentsWithType(const std::function<bool(T*)>& filter=always_return_true) const;

protected:
  Entity(const Uuid<Entity>& uuid);
  ~Entity();

private:
  QVector<Entity::Component*> _alLComponents;
  QVector<Entity::SpatialComponent*> _rootSpatialComponents;
};




class Entity::Component : public scene::Object
{
public:
  Component(const Uuid<Component>& uuid);
};


class Entity::SpatialComponent : public Component
{
public:
  const bool isMovable : 1;

  SpatialComponent(const Uuid<SpatialComponent>& uuid, bool isMovable);
};


} // namespace scene
} // namespace glrt

DECLARE_BASECLASS(glrt::scene::Object, glrt::scene::Entity);

#endif // GLRT_SCENE_ENTITY_H
