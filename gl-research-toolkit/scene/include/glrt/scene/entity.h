#ifndef GLRT_SCENE_ENTITY_H
#define GLRT_SCENE_ENTITY_H

#include <glrt/dependencies.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/coord-frame.h>

namespace glrt {
namespace scene {


class Scene;
class Entity final
{
public:
  class ModularAttribute;
  class Component;

  Scene& scene;
  const Uuid<Entity> uuid;

  template<typename T>
  QVector<T*> allModularAttributeWithType(const std::function<bool(T*)>& filter=always_return_true) const;
  template<typename T>
  QVector<T*> allComponentsWithType(const std::function<bool(T*)>& filter=always_return_true) const;

protected:
  Entity(Scene& scene, const Uuid<Entity>& uuid);
  ~Entity();

protected:
  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);

private:
  QVector<Entity::ModularAttribute*> _allModularAttributes;
  Entity::Component* _rootComponent;
};


class Entity::ModularAttribute
{
public:
  Entity& entity;
  Uuid<ModularAttribute> uuid;

  ModularAttribute(Entity& entity, const Uuid<ModularAttribute>& uuid);
  virtual ~ModularAttribute();
};


class Entity::Component
{
public:
  Entity& entity;
  Uuid<Component> uuid;

  const bool isMovable : 1;

  CoordFrame localCoordFrame;
  CoordFrame globalCoordFrame() const;

  Component(Entity& entity, const Uuid<Component>& uuid, bool isMovable);
  virtual ~Component();

protected:
  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);
};


} // namespace scene
} // namespace glrt

#include "entity.inl"

#endif // GLRT_SCENE_ENTITY_H
