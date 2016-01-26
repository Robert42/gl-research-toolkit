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

  resources::ResourceManager& resourceManager();

private:
  QVector<ModularAttribute*> _allModularAttributes;
  Component* _rootComponent;
};


class Node::ModularAttribute : public QObject
{
  Q_OBJECT
public:
  Node& entity;
  const Uuid<ModularAttribute> uuid;

  ModularAttribute(Node& entity, const Uuid<ModularAttribute>& uuid);
  virtual ~ModularAttribute();
};


class Node::Component : public QObject
{
  Q_OBJECT
public:
  Node& node;
  Component* const parent;
  const Uuid<Component> uuid;

  const bool isMovable : 1;

  Component(Node& node, Component* parent, const Uuid<Component>& uuid, bool isMovable);
  virtual ~Component();

  const QVector<Component*>& children() const;
  void collectSubtree(QVector<Component*>* subTree);

  CoordFrame localCoordFrame() const;
  CoordFrame globalCoordFrame() const;

  void set_localCoordFrame(const CoordFrame& coordFrame);

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

protected:
  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);

private:
  CoordFrame _localCoordFrame;

  QVector<Component*> _children;
};


} // namespace scene
} // namespace glrt

#include "node.inl"

#endif // GLRT_SCENE_ENTITY_H
