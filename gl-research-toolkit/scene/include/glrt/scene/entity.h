#ifndef GLRT_SCENE_ENTITY_H
#define GLRT_SCENE_ENTITY_H

#include <glrt/dependencies.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/coord-frame.h>

namespace glrt {
namespace scene {

class Scene;
// #TOOD: rename to Node?
class Entity final
{
public:
  class ModularAttribute;
  class Component;

  Scene& scene;
  const Uuid<Entity> uuid;

  CoordFrame globalCoordFrame() const;

  template<typename T>
  QVector<T*> allModularAttributeWithType(const std::function<bool(T*)>& filter=always_return_true) const;
  template<typename T>
  QVector<T*> allComponentsWithType(const std::function<bool(T*)>& filter=always_return_true) const;

  const QVector<ModularAttribute*>& allModularAttributes();

  QVector<Component*> allComponents() const;
  Component* rootComponent() const;

protected:
  Entity(Scene& scene, const Uuid<Entity>& uuid);
  ~Entity();

protected:
  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);

private:
  QVector<ModularAttribute*> _allModularAttributes;
  Component* _rootComponent;
};


class Entity::ModularAttribute
{
public:
  Entity& entity;
  const Uuid<ModularAttribute> uuid;

  ModularAttribute(Entity& entity, const Uuid<ModularAttribute>& uuid);
  virtual ~ModularAttribute();
};


class Entity::Component
{
public:
  Entity& entity;
  const Uuid<Component> uuid;

  const bool isMovable : 1;

  Component(Entity& entity, const Uuid<Component>& uuid, bool isMovable);
  virtual ~Component();

  Component* parent() const;
  void setParent(Component* component);

  QVector<Component*> children() const;
  void collectSubtree(QVector<Component*>* subTree);

  CoordFrame localCoordFrame() const;
  CoordFrame globalCoordFrame() const;

protected:
  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);

private:
  CoordFrame _localCoordFrame;

  Component* _parent = nullptr;
  QVector<Component*> _children;
};


} // namespace scene
} // namespace glrt

#include "entity.inl"

#endif // GLRT_SCENE_ENTITY_H
