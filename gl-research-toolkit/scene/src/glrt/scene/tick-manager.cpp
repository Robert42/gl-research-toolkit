#include <glrt/scene/tick-manager.h>

namespace glrt {
namespace scene {



TickManager::TickManager(QObject* parent)
  : QObject(parent)
{

}

void TickManager::tick(float deltaTime)
{
  this->deltaTime = deltaTime;
  updateObjects();

  fragmented_array.iterate(this);
}

void TickManager::addTickingObject(TickingObject* tickingObject)
{
  notYetAddedTickingPointers.insert(tickingObject);
}

void TickManager::updateObjects()
{
  for(TickingObject* deletedObject : deletedObjects)
    fragmented_array.remove(deletedObject);
  deletedObjects.clear();

  QSet<QPointer<TickingObject>> notYetAddedTickingPointers(std::move(this->notYetAddedTickingPointers));
  for(const QPointer<TickingObject>& ptr : notYetAddedTickingPointers)
  {
    if(ptr.isNull())
      continue;

    TickingObject* tickingObject = ptr.data();

    tickingObject->updateTickDependencyDepth();

    connect(tickingObject, &TickingObject::tickDependencyDepthChanged, this, &TickManager::updateObject);
    connect(tickingObject, &QObject::destroyed, this, &TickManager::removeObject);

    TickingObject::TickTraits traits = tickingObject->tickTraits();

    if(traits == TickingObject::TickTraits::NoTick)
      continue;

    fragmented_array.append_copy(tickingObject);
  }

  fragmented_array.updateSegments(this);
}

/*!
\threadsafe
*/
void TickManager::removeObject(QObject* object)
{
  QMutexLocker l(&mutex);
  deletedObjects.insert(static_cast<TickingObject*>(object));
}

/*!
\threadsafe
*/
void TickManager::updateObject(TickingObject* object)
{
  QMutexLocker l(&mutex);
  fragmented_array.orderChangedForValue(object);
}


} // namespace scene
} // namespace glrt
