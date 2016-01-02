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

protected:
  Entity(const Uuid<Entity>& uuid);
  ~Entity();

private:
  QVector<Entity::ModularAttribute*> _allModularAttributes;
  Entity::Component* _rootComponent;
};




class Entity::ModularAttribute : public scene::Object
{
public:
  ModularAttribute(const Uuid<ModularAttribute>& uuid);
};


class Entity::Component : public scene::Object
{
public:
  const bool isMovable : 1;

  CoordFrame localCoordFrame;
  CoordFrame get_globalCoordFrame() const;

  Component(const Uuid<Component>& uuid, bool isMovable);
};


} // namespace scene
} // namespace glrt

DECLARE_BASECLASS(glrt::scene::Object, glrt::scene::Entity);

#endif // GLRT_SCENE_ENTITY_H
