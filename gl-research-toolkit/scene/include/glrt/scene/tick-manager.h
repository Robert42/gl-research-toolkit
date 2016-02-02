#ifndef GLRT_SCENE_TICKMANAGER_H
#define GLRT_SCENE_TICKMANAGER_H

#include <glrt/scene/ticking-object.h>
#include <glrt/toolkit/fragmented-array.h>

namespace glrt {
namespace scene {

class TickManager : public QObject
{
  Q_OBJECT
public:
  TickManager(QObject* parent=nullptr);

  void tick(float deltaTime);

private:
  friend class TickingObject;
  QSet<QPointer<TickingObject>> notYetAddedTickingPointers;

  void addTickingObject(TickingObject* tickingObject);
  void updateObjects();

private slots:
  void removeObject(QObject* object);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_TICKMANAGER_H
