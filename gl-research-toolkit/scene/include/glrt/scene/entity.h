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
  class ModularAttribute;
  class Component;

  template<typename T>
  QVector<T*> allModularAttributeWithType(const std::function<bool(T*)>& filter=always_return_true) const;
  template<typename T>
  QVector<T*> allComponentsWithType(const std::function<bool(T*)>& filter=always_return_true) const;

  void addModularAttribute(ModularAttribute* modularAttribute);
  void removeModularAttribute(ModularAttribute* modularAttribute);

protected:
  Entity(const Uuid<Entity>& uuid);
  ~Entity();

protected:
  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);

private:
  QVector<Entity::ModularAttribute*> _allModularAttributes;
  Entity::Component* _rootComponent;
};


class Entity::ModularAttribute : public scene::Object
{
public:
  ModularAttribute(const Uuid<ModularAttribute>& uuid);

  void set_entity(const ref<Entity>& entity);
  const weakref<Entity>& get_entity();

protected:
  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);

private:
  weakref<Entity> entity;
};


class Entity::Component : public scene::Object
{
public:
  const bool isMovable : 1;

  CoordFrame localCoordFrame;
  CoordFrame get_globalCoordFrame() const;

  Component(const Uuid<Component>& uuid, bool isMovable);

protected:
  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);
};


} // namespace scene
} // namespace glrt

DECLARE_BASECLASS(glrt::scene::Object, glrt::scene::Entity);
DECLARE_BASECLASS(glrt::scene::Object, glrt::scene::Entity::ModularAttribute);
DECLARE_BASECLASS(glrt::scene::Object, glrt::scene::Entity::Component);

#include "entity.inl"

#endif // GLRT_SCENE_ENTITY_H
