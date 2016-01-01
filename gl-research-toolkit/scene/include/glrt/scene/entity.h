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
  class LogicModule;
  class Component;

  typedef Uuid<LogicModule> LogicModuleUuid;
  typedef Uuid<Component> ComponentUuid;


protected:
  Entity(const Uuid<Entity>& uuid);
  ~Entity();

private:
  QVector<Entity::Component*> _components;
};




class Entity::LogicModule : public scene::Object
{
public:
  LogicModule(const Uuid<LogicModule>& uuid);
};


class Entity::Component : public scene::Object
{
public:
  const bool isMovable : 1;

  Component(const Uuid<Component>& uuid, bool isMovable);
};


} // namespace scene
} // namespace glrt

DECLARE_BASECLASS(glrt::scene::Object, glrt::scene::Entity);

#endif // GLRT_SCENE_ENTITY_H
