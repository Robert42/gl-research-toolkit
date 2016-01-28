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
  class TickingObject;
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

class Node::TickingObject : public QObject
{
  Q_OBJECT
public:
  virtual void tick(float timeDelta) const;

  bool tickDependsOn(const Component* other) const;
  int updateTickDependencyDepth();

signals:
  void tickDependencyDepthChanged(TickingObject* sender);

protected:
  struct TickTraits;

  template<typename T>
  struct DependencySet;

  typedef DependencySet<TickingObject> TickDependencySet;

  TickingObject();

  void collectDependencies(TickDependencySet* dependencySet) const;

  virtual TickTraits tickTraits() const;
  virtual void collectTickDependencies(TickDependencySet* dependencySet) const;

private:
  int _tickDependencyDepth;
};


class Node::ModularAttribute : public TickingObject
{
  Q_OBJECT
public:
  Node& node;
  const Uuid<ModularAttribute> uuid;

  ModularAttribute(Node& node, const Uuid<ModularAttribute>& uuid);
  virtual ~ModularAttribute();
};


class Node::Component : public TickingObject
{
  Q_OBJECT
  Q_PROPERTY(bool movable READ movable WRITE setMovable NOTIFY movableChanged)
public:

  Node& node;
  Component* const parent;
  const Uuid<Component> uuid;

  Component(Node& node, Component* parent, const Uuid<Component>& uuid);
  virtual ~Component();

  const QVector<Component*>& children() const;
  void collectSubtree(QVector<Component*>* subTree);

  bool movable() const;
  void setMovable(bool movable);

  CoordFrame localCoordFrame() const;
  CoordFrame globalCoordFrame() const;

  virtual CoordFrame calcGlobalCoordFrame() const;
  bool hasCustomGlobalCoordUpdater() const;

  void set_localCoordFrame(const CoordFrame& coordFrame);

  bool coordDependsOn(const Component* other) const;
  int updateCoordDependencyDepth();

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

signals:
  void coordDependencyDepthChanged(Component* sender);
  void movableChanged(Component* sender);

protected:
  typedef DependencySet<Component> CoordDependencySet;
  friend struct DependencySet<Component>;

  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);

  template<typename T, T*>
  static void registerCreateMethod(AngelScript::asIScriptEngine* engine, const char* type, const char* arguments);

  void collectDependencies(TickDependencySet* dependencySet) const;
  void collectDependencies(CoordDependencySet* dependencySet) const;

  virtual void collectCoordDependencies(CoordDependencySet* dependencySet) const;

private:
  template<typename T>
  struct _create_method_helper;

  template<typename T_Component, typename... T_Args>
  struct _create_method_helper<T_Component*(Node& node, Component* parent, T_Args...)>;

  template<typename T, T*>
  static void _registerCreateMethod(AngelScript::asIScriptEngine* engine, const char* type, const std::string& function_name, const char* arguments);

  CoordFrame _localCoordFrame;

  bool _movable : 1;

  QVector<Component*> _children;
  int _coorddependencyDepth;
  int _coordinateIndex;
};



} // namespace scene
} // namespace glrt

#include "node.inl"

#endif // GLRT_SCENE_NODE_H
