#ifndef GLRT_SCENE_TICKMANAGER_H
#define GLRT_SCENE_TICKMANAGER_H

#include <glrt/scene/ticking-object.h>

namespace glrt {
namespace scene {

class TickManager : public QObject
{
  Q_OBJECT
public:
  TickManager(QObject* parent=nullptr);

  void tick(float deltaTime);

#if 0
private:
  friend class TickingObject;
  friend struct implementation::TickManager_TickObjectArray;

  implementation::TickManager_TickObjectArray::type fragmented_array;
  QSet<QPointer<TickingObject>> notYetAddedTickingPointers;
  QSet<TickingObject*> deletedObjects;
  QReadWriteLock mutexFragmentedArray;
  float deltaTime;

  void addTickingObject(TickingObject* tickingObject);
  void updateObjects();

private slots:
  void removeObject(QObject* object);
  void updateObject(TickingObject* object);
#endif
};


} // namespace scene
} // namespace glrt

#if 0
#include "implementation/tick-manager-tickorder-array.inl"
#endif

#endif // GLRT_SCENE_TICKMANAGER_H
