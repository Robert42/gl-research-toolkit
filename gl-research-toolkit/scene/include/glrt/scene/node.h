#ifndef GLRT_SCENE_NODE_H
#define GLRT_SCENE_NODE_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/array.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/coord-frame.h>
#include <glrt/scene/aabb.h>
#include <glrt/scene/ticking-object.h>

namespace glrt {
namespace scene {

class ComponentWithAABB;
class SceneLayer;
class Node final : public QObject
{
  Q_OBJECT
public:
  class ModularAttribute;
  class Component;

  const Uuid<Node> uuid;

  Node(SceneLayer& sceneLayer, const Uuid<Node>& uuid);
  ~Node();

  Scene& scene();
  const Scene& scene()const;
  SceneLayer& sceneLayer();
  const SceneLayer& sceneLayer()const;
  resources::ResourceManager& resourceManager();
  const resources::ResourceManager& resourceManager() const;

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


private:
  SceneLayer& _sceneLayer;
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

  Scene& scene();
  const Scene& scene()const;
  SceneLayer& sceneLayer();
  const SceneLayer& sceneLayer()const;
  resources::ResourceManager& resourceManager();
  const resources::ResourceManager& resourceManager() const;
};


class Node::Component : public TickingObject
{
  Q_OBJECT
  Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
public:
  enum class DataClass
  {
    EMPTY = 0x0,
    EMPTY_STATIC = 0x1,
    TRANSFORMATION = 0x2,
    TRANSFORMATION_STATIC = TRANSFORMATION | EMPTY_STATIC,
    SPHERELIGHT = 0x4,
    SPHERELIGHT_STATIC = SPHERELIGHT | EMPTY_STATIC,
    RECTLIGHT = 0x6,
    RECTLIGHT_STATIC = RECTLIGHT | EMPTY_STATIC,
    STATICMESH = 0x8,
    STATICMESH_STATIC = STATICMESH | EMPTY_STATIC,
    VOXELGRID = 0x10,
    VOXELGRID_STATIC = VOXELGRID | EMPTY_STATIC,
    CAMERA = 0x12,
    CAMERA_STATIC = CAMERA | EMPTY_STATIC,
  };

  friend DataClass operator&(DataClass a, DataClass b){return DataClass(quint32(a)&quint32(b));}
  friend DataClass operator|(DataClass a, DataClass b){return DataClass(quint32(a)|quint32(b));}

  Node& node;
  Component* const parent;
  const Uuid<Component> uuid;
  const DataClass data_class;
  int data_index = -1;

  Component(Node& node, Component* parent, const Uuid<Component>& uuid, DataClass data_class);
  virtual ~Component();

  Scene& scene();
  const Scene& scene()const;
  SceneLayer& sceneLayer();
  const SceneLayer& sceneLayer()const;
  resources::ResourceManager& resourceManager();
  const resources::ResourceManager& resourceManager() const;

  const QVector<Component*>& children() const;
  void collectSubtree(QVector<Component*>* subTree);

  bool isStatic() const;
  bool visible() const;

  bool hasAABB() const;

  CoordFrame localCoordFrame() const;
  CoordFrame globalCoordFrame() const;

  quint32 zIndex() const;

  CoordFrame updateGlobalCoordFrame();
  quint32 updateZIndex();
  virtual CoordFrame calcGlobalCoordFrameImpl() const;
  bool hasCustomGlobalCoordUpdater() const;

  void set_localCoordFrame(const CoordFrame& coordFrame);

  bool coordDependsOn(const Component* other) const;
  int updateCoordDependencyDepth();
  int coordDependencyDepth() const;

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

  static DataClass makeStatic(DataClass dataClass, bool makeStatic);

public slots:
  void setVisible(bool visible);
  void show(bool show=true);
  void hide(bool hide=true);

  void hideNowAndDeleteLater();

signals:
  void coordDependencyDepthChanged(Component* sender);
  void componentVisibilityChanged(Component* sender);
  void visibleChanged(bool);

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

  void hideInDestructor();

private:
  template<typename T>
  struct _create_method_helper;

  template<typename T_Component, typename... T_Args>
  struct _create_method_helper<T_Component*(Node& node, Component* parent, T_Args...)>;

  template<typename T, T*>
  static void _registerCreateMethod(AngelScript::asIScriptEngine* engine, const char* type, const std::string& function_name, const char* arguments);

  friend struct implementation::GlobalCoordArrayOrder;
  friend class ComponentWithAABB;
  CoordFrame _localCoordFrame;
  CoordFrame _globalCoordFrame;
  quint32 _zIndex = 0;

  bool _visible : 1;
  bool _parentVisible : 1;
  bool _hiddenBecauseDeletedNextFrame : 1;
  bool _hasAABB : 1;

  QVector<Component*> _children;
  int _coorddependencyDepth;
  int _coordinateIndex;

  void updateVisibility(bool prevVisibility);
};


class ComponentWithAABB : public Node::Component
{
public:
  AABB localAabb;

  ComponentWithAABB(Node& node, Component* parent, const Uuid<Component>& uuid, DataClass dataClass);
  ~ComponentWithAABB();

  AABB globalAABB() const;

  void expandSceneAABB();
};


} // namespace scene
} // namespace glrt

#include "node.inl"

#endif // GLRT_SCENE_NODE_H
