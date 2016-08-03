#ifndef GLRT_SCENE_GLOBALCOORDUPDATER_H
#define GLRT_SCENE_GLOBALCOORDUPDATER_H

#include <glrt/scene/node.h>

namespace glrt {
namespace scene {


class GlobalCoordUpdater final : public QObject
{
  Q_OBJECT
public:
  GlobalCoordUpdater();

  void updateCoordinates();

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
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_GLOBALCOORDUPDATER_H
