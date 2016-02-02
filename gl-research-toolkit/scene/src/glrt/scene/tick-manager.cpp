#include <glrt/scene/tick-manager.h>

namespace glrt {
namespace scene {

TickManager::TickManager(QObject* parent)
  : QObject(parent)
{

}

void TickManager::tick(float deltaTime)
{
  updateObjects();
}

void TickManager::addTickingObject(TickingObject* tickingObject)
{
  notYetAddedTickingPointers.insert(tickingObject);

  fragmented_array.iterate();
}

void TickManager::updateObjects()
{
  QSet<QPointer<TickingObject>> notYetAddedTickingPointers(std::move(this->notYetAddedTickingPointers));
  for(const QPointer<TickingObject>& ptr : notYetAddedTickingPointers)
  {
    if(ptr.isNull())
      continue;

    TickingObject* tickingObject = ptr.data();

    connect(tickingObject, &QObject::destroyed, this, &TickManager::removeObject);

    TickingObject::TickTraits traits = tickingObject->tickTraits();

    if(traits == TickingObject::TickTraits::NoTick)
      continue;

    fragmented_array.append_copy(tickingObject);
  }

  fragmented_array.update();
}

void TickManager::removeObject(QObject* object)
{
  fragmented_array.remove(static_cast<TickingObject*>(object));
}

} // namespace scene
} // namespace glrt
