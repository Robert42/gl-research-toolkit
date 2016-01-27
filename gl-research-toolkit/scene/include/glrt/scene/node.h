#ifndef GLRT_SCENE_NODE_H
#define GLRT_SCENE_NODE_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/array.h>
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
  Node& node;
  const Uuid<ModularAttribute> uuid;

  ModularAttribute(Node& node, const Uuid<ModularAttribute>& uuid);
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

  Component(Node& node, Component* parent, const Uuid<Component>& uuid);
  virtual ~Component();

  const QVector<Component*>& children() const;
  void collectSubtree(QVector<Component*>* subTree);

  CoordFrame localCoordFrame() const;
  CoordFrame globalCoordFrame() const;

  void set_localCoordFrame(const CoordFrame& coordFrame);

  bool dependsOn(const Component* other) const;
  int updateDependencyDepth();

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

signals:
  void dependencyDepthChanged();

protected:
  struct DependencySet;

  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);

  template<typename T, T*>
  static void registerCreateMethod(AngelScript::asIScriptEngine* engine, const char* type, const char* arguments);

  virtual void collectDependencies(DependencySet* dependencySet) const;

private:
  template<typename T>
  struct _create_method_helper;

  template<typename T_Component, typename... T_Args>
  struct _create_method_helper<T_Component*(Node& node, Component* parent, T_Args...)>;

  template<typename T, T*>
  static void _registerCreateMethod(AngelScript::asIScriptEngine* engine, const char* type, const std::string& function_name, const char* arguments);

  CoordFrame _localCoordFrame;

  QVector<Component*> _children;
  int _dependencyDepth;
};


struct Node::Component::DependencySet final
{
  QSet<const Component*> componentsWithCycles;

  DependencySet(const Component* component);

  void addDependency(const Component* component);

  bool dependsOn(const Component* other) const;
  bool hasCycles() const;
  int depth() const;

private:
  QSet<const Component*> visitedDependencies;
  QQueue<const Component*> queuedDependencies;
  int _depth;
};



} // namespace scene
} // namespace glrt

#include "node.inl"

#endif // GLRT_SCENE_NODE_H
