#ifndef GLRT_SCENE_NODE_H
#define GLRT_SCENE_NODE_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/array.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/coord-frame.h>
#include <glrt/scene/aabb.h>
#include <glrt/toolkit/dependency-set.h>

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


class Node::ModularAttribute : public QObject
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


class Node::Component : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
public:
  enum class DataClass : quint8
  {
    EMPTY = 0,
    SPHERELIGHT,
    RECTLIGHT,
    STATICMESH,
    VOXELGRID,
    CAMERA,

    NUM_DATA_CLASSES,
    MASK = 0x7f,
    MOVABLE = 0x80
  };

  friend DataClass operator&(DataClass a, DataClass b){return DataClass(quint32(a)&quint32(b));}
  friend DataClass operator|(DataClass a, DataClass b){return DataClass(quint32(a)|quint32(b));}

  struct DataIndex
  {
    const DataClass data_class;
    quint8 _padding;

    quint16 array_index;
  };

  Node& node;
  Component* const parent;
  const Uuid<Component> uuid;
  DataIndex data_index;

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

  bool isMovable() const;
  bool visible() const;

  bool hasAABB() const;

  const CoordFrame& localCoordFrame() const;

  CoordFrame globalCoordFrame() const;

#if 0
  quint32 zIndex() const;

  CoordFrame updateGlobalCoordFrame();
  quint32 updateZIndex();
  virtual CoordFrame calcGlobalCoordFrameImpl() const;
  bool hasCustomGlobalCoordUpdater() const;
#endif


  bool coordDependsOn(const Component* other) const;
  int updateCoordDependencyDepth();
  int coordDependencyDepth() const;

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

  static DataClass makeMovable(DataClass dataClass, bool makeMovable);

public slots:
  void setVisible(bool visible);
  void show(bool show=true);
  void hide(bool hide=true);

  void hideNowAndDeleteLater();

signals:
  void componentVisibilityChanged(Component* sender);
  void visibleChanged(bool);

protected:
  typedef DependencySet<Component> CoordDependencySet;
  friend struct DependencySet<Component>;

  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);

  template<typename T, T*>
  static void registerCreateMethod(AngelScript::asIScriptEngine* engine, const char* type, const char* arguments);

  void collectDependencies(CoordDependencySet* dependencySet) const;

  virtual void collectCoordDependencies(CoordDependencySet* dependencySet) const;

  void hideInDestructor();

  void set_localCoordFrame(const CoordFrame& coordFrame);

private:
  template<typename T>
  struct _create_method_helper;

  template<typename T_Component, typename... T_Args>
  struct _create_method_helper<T_Component*(Node& node, Component* parent, T_Args...)>;

  template<typename T, T*>
  static void _registerCreateMethod(AngelScript::asIScriptEngine* engine, const char* type, const std::string& function_name, const char* arguments);

  friend struct implementation::GlobalCoordArrayOrder;
  friend class ComponentWithAABB;
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
