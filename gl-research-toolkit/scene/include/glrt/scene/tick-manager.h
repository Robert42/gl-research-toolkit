#ifndef GLRT_SCENE_TICKMANAGER_H
#define GLRT_SCENE_TICKMANAGER_H

#include <glrt/scene/ticking-object.h>
#include <glrt/toolkit/fragmented-array.h>

#include "implementation/tick-manager-tickorder-array.h"

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
  friend struct implementation::TickManager_TickObjectArray;

  implementation::TickManager_TickObjectArray objectArray;
  QSet<QPointer<TickingObject>> notYetAddedTickingPointers;
  QSet<TickingObject*> deletedObjects;
  QMutex mutex;
  float deltaTime;

  void addTickingObject(TickingObject* tickingObject);
  void updateObjects();

private slots:
  void removeObject(QObject* object);
  void updateObject(TickingObject* object);
};

} // namespace scene
} // namespace glrt

#include "implementation/tick-manager-tickorder-array.inl"

#endif // GLRT_SCENE_TICKMANAGER_H
