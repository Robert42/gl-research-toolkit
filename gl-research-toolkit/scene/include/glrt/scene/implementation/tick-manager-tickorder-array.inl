#ifndef GLRT_SCENE_IMPLEMENTATION_TICKMANAGER_TICKORDER_INL
#define GLRT_SCENE_IMPLEMENTATION_TICKMANAGER_TICKORDER_INL

#include <glrt/scene/tick-manager.h>

namespace glrt {
namespace scene {
namespace implementation {


void TickManager_TickObjectArray::TickCaller::handle_value(TickingObject**, int, TickManager*)
{
}

bool TickManager_TickObjectArray::TickCaller::valueLessThan(TickingObject* a, TickingObject* b)
{
  return a < b;
}


bool TickManager_TickObjectArray::DependencyDepthHandler::segmentLessThan(const TickingObject* a, const TickingObject* b)
{
  return classify(a) < classify(b);
}

int TickManager_TickObjectArray::DependencyDepthHandler::classify(const TickingObject* tickingObject)
{
  return tickingObject->tickDependencyDepth();
}

int TickManager_TickObjectArray::DependencyDepthHandler::segment_as_index(int i)
{
  return i;
}

int TickManager_TickObjectArray::DependencyDepthHandler::segment_from_index(int i)
{
  return i;
}

void TickManager_TickObjectArray::DependencyDepthHandler::handle_new_segment(TickingObject** objects, int begin, int end, int depth, TickManager* tickManager)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);
  Q_UNUSED(depth);
  Q_UNUSED(tickManager);
}

void TickManager_TickObjectArray::DependencyDepthHandler::handle_end_segment(TickingObject** objects, int begin, int end, int depth, TickManager* tickManager)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);
  Q_UNUSED(depth);
  Q_UNUSED(tickManager);
}


bool TickManager_TickObjectArray::MultithreadingHandler::segmentLessThan(const TickingObject* a, const TickingObject* b)
{
  return classify(a) < classify(b);
}

TickingObject::TickTraits TickManager_TickObjectArray::MultithreadingHandler::classify(const TickingObject* tickingObject)
{
  return tickingObject->tickTraits();
}

int TickManager_TickObjectArray::MultithreadingHandler::segment_as_index(TickingObject::TickTraits i)
{
  return static_cast<int>(i);
}

TickingObject::TickTraits TickManager_TickObjectArray::MultithreadingHandler::segment_from_index(int i)
{
  return static_cast<TickingObject::TickTraits>(i);
}

void TickManager_TickObjectArray::MultithreadingHandler::handle_new_segment(TickingObject** objects, int begin, int end, TickingObject::TickTraits tickTraits, TickManager* tickManager)
{
  switch(tickTraits)
  {
  case TickingObject::TickTraits::Multithreaded:
    // #ISSUE-61 OMP
    for(int i=begin; i<end; ++i)
    {
      if(tickManager->deletedObjects.contains(objects[i]))
        continue;
      objects[i]->tick(tickManager->deltaTime);
    }
    break;
  case TickingObject::TickTraits::OnlyMainThread:
    for(int i=begin; i<end; ++i)
      if(!tickManager->deletedObjects.contains(objects[i]))
        objects[i]->tick(tickManager->deltaTime);
    break;
  default:
    Q_UNREACHABLE();
  }
}

void TickManager_TickObjectArray::MultithreadingHandler::handle_end_segment(TickingObject** objects, int begin, int end, TickingObject::TickTraits tickTraits, TickManager* tickManager)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);
  Q_UNUSED(tickTraits);
  Q_UNUSED(tickManager);
}


} // namespace implementation
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_IMPLEMENTATION_TICKMANAGER_TICKORDER_INL
