#ifndef GLRT_SCENE_GLOBALCOORDUPDATER_H
#define GLRT_SCENE_GLOBALCOORDUPDATER_H

#include <glrt/toolkit/fragmented-array.h>
#include <glrt/scene/node.h>

#include "implementation/global-coord-array-order.h"

namespace glrt {
namespace scene {


class GlobalCoordUpdater : public QObject
{
  Q_OBJECT
public:
  GlobalCoordUpdater();

  void updateCoordinages();

private:
  friend class Node::Component;
  friend struct implementation::GlobalCoordArrayOrder;

  implementation::GlobalCoordArrayOrder::type fragmented_array;
  QSet<QPointer<TickingObject>> notYetAddedComponents;
  QMutex mutex;

  void addComponent(Node::Component* component);

  void updateArray();

private slots:
  void removeObject(QObject*);
  void movabilityChanged(Node::Component* component);
};


} // namespace scene
} // namespace glrt

#include "implementation/global-coord-array-order.inl"

#endif // GLRT_SCENE_GLOBALCOORDUPDATER_H
