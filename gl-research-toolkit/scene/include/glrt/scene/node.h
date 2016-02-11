#ifndef GLRT_SCENE_NODE_H
#define GLRT_SCENE_NODE_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/array.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/coord-frame.h>
#include <glrt/scene/ticking-object.h>

namespace glrt {
namespace scene {

class SceneLayer;
class Node final : public QObject
{
  Q_OBJECT
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
  QVector<T*> allModularAttributeWithType(const std::function<bool(T*)>& filter=always_return_true<T*>) const;
  template<typename T>
  QVector<T*> allComponentsWithType(const std::function<bool(T*)>& filter=always_return_true<T*>) const;

  const QVector<ModularAttribute*>& allModularAttributes();

  QVector<Component*> allComponents() const;
  Component* rootComponent() const;

  resources::ResourceManager& resourceManager();

private:
  QVector<ModularAttribute*> _allModularAttributes;
  Component* _rootComponent;
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
  Q_PROPERTY(bool mayBecomeMovable READ mayBecomeMovable WRITE setMayBecomeMovable)
  Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
public:

  Node& node;
  Component* const parent;
  const Uuid<Component> uuid;

  Component(Node& node, Component* parent, const Uuid<Component>& uuid);
  virtual ~Component();

  const QVector<Component*>& children() const;
  void collectSubtree(QVector<Component*>* subTree);

  bool movable() const;
  bool mayBecomeMovable() const;
  bool visible() const;

  CoordFrame localCoordFrame() const;
  CoordFrame globalCoordFrame() const;

  CoordFrame updateGlobalCoordFrame();
  virtual CoordFrame calcGlobalCoordFrameImpl() const;
  bool hasCustomGlobalCoordUpdater() const;

  void set_localCoordFrame(const CoordFrame& coordFrame);

  bool coordDependsOn(const Component* other) const;
  int updateCoordDependencyDepth();
  int coordDependencyDepth() const;

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

public slots:
  void setMovable(bool movable);
  void setVisible(bool visible);
  void show(bool show=true);
  void hide(bool hide=true);

  void hideNowAndDeleteLater();

signals:
  void coordDependencyDepthChanged(Component* sender);
  void componentMovabilityChanged(Component* sender);
  void componentVisibilityChanged(Component* sender);
  void visibleChanged(bool);
  void movableChanged(bool);

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

  void setMayBecomeMovable(bool mayBecomeMovable);

  void hideInDestructor();

private:
  template<typename T>
  struct _create_method_helper;

  template<typename T_Component, typename... T_Args>
  struct _create_method_helper<T_Component*(Node& node, Component* parent, T_Args...)>;

  template<typename T, T*>
  static void _registerCreateMethod(AngelScript::asIScriptEngine* engine, const char* type, const std::string& function_name, const char* arguments);

  friend struct implementation::GlobalCoordArrayOrder;
  CoordFrame _localCoordFrame;
  CoordFrame _globalCoordFrame;

  bool _movable : 1;
  bool _mayBecomeMovable : 1;
  bool _visible : 1;
  bool _parentVisible : 1;
  bool _hiddenBecauseDeletedNextFrame : 1;

  QVector<Component*> _children;
  int _coorddependencyDepth;
  int _coordinateIndex;

  void updateVisibility(bool prevVisibility);
};



} // namespace scene
} // namespace glrt

#include "node.inl"

#endif // GLRT_SCENE_NODE_H
