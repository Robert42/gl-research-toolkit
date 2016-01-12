#ifndef GLRT_SCENE_ENTITY_H
#define GLRT_SCENE_ENTITY_H

#include <glrt/dependencies.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/coord-frame.h>

namespace glrt {
namespace scene {

class SceneLayer;
class Node final
{
public:
  class ModularAttribute;
  class Component;

  SceneLayer& sceneLayer;
  const Uuid<Node> uuid;

  Node(SceneLayer& sceneLayer, const Uuid<Node>& uuid);
  ~Node();

  CoordFrame globalCoordFrame() const;

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

  template<typename T>
  QVector<T*> allModularAttributeWithType(const std::function<bool(T*)>& filter=always_return_true) const;
  template<typename T>
  QVector<T*> allComponentsWithType(const std::function<bool(T*)>& filter=always_return_true) const;

  const QVector<ModularAttribute*>& allModularAttributes();

  QVector<Component*> allComponents() const;
  Component* rootComponent() const;

private:
  QVector<ModularAttribute*> _allModularAttributes;
  Component* _rootComponent;
};


class Node::ModularAttribute
{
public:
  Node& entity;
  const Uuid<ModularAttribute> uuid;

  ModularAttribute(Node& entity, const Uuid<ModularAttribute>& uuid);
  virtual ~ModularAttribute();
};


class Node::Component
{
public:
  Node& entity;
  const Uuid<Component> uuid;

  const bool isMovable : 1;

  Component(Node& entity, const Uuid<Component>& uuid, bool isMovable);
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

#include "node.inl"

#endif // GLRT_SCENE_ENTITY_H
