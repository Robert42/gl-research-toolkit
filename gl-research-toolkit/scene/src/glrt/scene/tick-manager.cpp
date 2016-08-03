#include <glrt/scene/tick-manager.h>

namespace glrt {
namespace scene {



TickManager::TickManager(QObject* parent)
  : QObject(parent)
{

}

void TickManager::tick(float deltaTime)
{
#if 0
  this->deltaTime = deltaTime;
  updateObjects();

  fragmented_array.iterate(this);
#endif
}

#if 0
void TickManager::addTickingObject(TickingObject* tickingObject)
{
  notYetAddedTickingPointers.insert(tickingObject);
}

void TickManager::updateObjects()
{
  fragmented_array.removeMultiple(deletedObjects);
  deletedObjects.clear();

  QSet<QPointer<TickingObject>> notYetAddedTickingPointers(std::move(this->notYetAddedTickingPointers));
  for(const QPointer<TickingObject>& ptr : notYetAddedTickingPointers)
  {
    if(ptr.isNull())
      continue;

    TickingObject* tickingObject = ptr.data();

    TickingObject::TickTraits traits = tickingObject->tickTraits();

    if(traits == TickingObject::TickTraits::NoTick)
      continue;

    tickingObject->updateTickDependencyDepth();

    connect(tickingObject, &TickingObject::tickDependencyDepthChanged, this, &TickManager::updateObject);
    connect(tickingObject, &QObject::destroyed, this, &TickManager::removeObject);

    fragmented_array.append_copy(tickingObject);
  }

  fragmented_array.updateSegments(this);
}


void TickManager::removeObject(QObject* object)
{
  // TODO: add check, that this isn't called during TickManager::tick
  deletedObjects.insert(static_cast<TickingObject*>(object));
}

/*!
\threadsafe
*/
void TickManager::updateObject(TickingObject* object)
{
  QWriteLocker l(&mutexFragmentedArray);
  fragmented_array.orderChangedForValue(object);
}
#endif


} // namespace scene
} // namespace glrt
