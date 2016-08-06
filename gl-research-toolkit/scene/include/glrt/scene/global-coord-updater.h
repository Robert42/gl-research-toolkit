#ifndef GLRT_SCENE_GLOBALCOORDUPDATER_H
#define GLRT_SCENE_GLOBALCOORDUPDATER_H

#include <glrt/scene/node.h>

namespace glrt {
namespace scene {


class GlobalCoordUpdater final : public QObject
{
  Q_OBJECT
public:
  GlobalCoordUpdater(Scene* scene);
  ~GlobalCoordUpdater();

  void updateCoordinates();

public slots:
  void addComponent(Node::Component* component);
  void removeComponent(Node::Component* component);

  // TODO:::::::::::::::::::::::::::
#if 0
private:
  friend class Node::Component;
  friend struct implementation::GlobalCoordArrayOrder;

  implementation::GlobalCoordArrayOrder::type fragmented_array;
  QMutex mutex;
  Array<QPointer<Node::Component>> staticComponentsToUpdate;

  void addComponent(Node::Component* component);

  void updateArray();

private slots:
  void removeObject(QObject*);
  void movabilityChanged(Node::Component* component);
  void dependencyDepthChanged(Node::Component* component);
#endif

private:
  typedef quint64 Bitfield;

  Scene& scene;

  Array<Array<Node::Component*>> notDynamicComponents_pending;
  Array<Array<Node::Component*>> dynamicComponents;
  Bitfield _need_resorting_not_dynamic = 0;
  Bitfield _need_resorting_dynamic = 0;

  Array<Node::Component*>& arrayFor(Node::Component* component);
  void needResortingFor(Node::Component* component);

  static void resort(Array<Array<Node::Component*>>* arrays, Bitfield* needResorting);
  void updateCoordinatesOf(Array<Array<Node::Component*>>& arrays);

  void copyLocalToGlobalCoordinates(const Array<Node::Component*>& arrays);
  void updateCoordinatesOf(const Array<Node::Component*>& array);
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_GLOBALCOORDUPDATER_H
