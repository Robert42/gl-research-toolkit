#ifndef GLRT_SCENE_IMPLEMENTATION_GLOBALCOORDARRAYORDER_INL
#define GLRT_SCENE_IMPLEMENTATION_GLOBALCOORDARRAYORDER_INL

#include <glrt/scene/global-coord-updater.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {
namespace implementation {

void GlobalCoordArrayOrder::UpdateCaller::handle_value(Node::Component**, int, extra_data_type)
{
}

bool GlobalCoordArrayOrder::UpdateCaller::valueLessThan(Node::Component* a, Node::Component* b)
{
  return a->zIndex()<b->zIndex();
}


bool GlobalCoordArrayOrder::DependencyDepthHandler::segmentLessThan(const Node::Component* a, const Node::Component* b)
{
  return a->coordDependencyDepth() < b->coordDependencyDepth();
}

int GlobalCoordArrayOrder::DependencyDepthHandler::classify(const Node::Component* component)
{
  return component->coordDependencyDepth();
}

int GlobalCoordArrayOrder::DependencyDepthHandler::segment_as_index(int i)
{
  return i;
}

int GlobalCoordArrayOrder::DependencyDepthHandler::segment_from_index(int i)
{
  return i;
}

void GlobalCoordArrayOrder::DependencyDepthHandler::handle_new_segment(Node::Component**, int , int, int, extra_data_type)
{
}

void GlobalCoordArrayOrder::DependencyDepthHandler::handle_end_segment(Node::Component**, int, int, int, extra_data_type)
{
}


bool GlobalCoordArrayOrder::HasCustomUpdaterHandler::segmentLessThan(const Node::Component* a, const Node::Component* b)
{
  return segment_as_index(classify(a)) < segment_as_index(classify(b));
}

GlobalCoordArrayOrder::UpdateType GlobalCoordArrayOrder::HasCustomUpdaterHandler::classify(const Node::Component* component)
{
  Q_ASSERT(component != nullptr);
  return component->hasCustomGlobalCoordUpdater() ? UpdateType::Custom : component->parent != nullptr ? UpdateType::WithParent : UpdateType::NoParent;
}

int GlobalCoordArrayOrder::HasCustomUpdaterHandler::segment_as_index(UpdateType updateType)
{
  return int(updateType);
}

GlobalCoordArrayOrder::UpdateType GlobalCoordArrayOrder::HasCustomUpdaterHandler::segment_from_index(int i)
{
  return UpdateType(glm::clamp(i, 0, 2));
}

void GlobalCoordArrayOrder::HasCustomUpdaterHandler::handle_new_segment(Node::Component** objects, int begin, int end, UpdateType updateType, extra_data_type coordManager)
{
  // #FIXME check, whethe only the movalb e components are updated here.

  Q_UNUSED(coordManager);

  // necessary check for if(objects[begin]->parent != nullptr)
  if(begin==end)
    return;

  switch(updateType)
  {
  case UpdateType::Custom:
    // #ISSUE-61 OMP
    for(int i=begin; i!=end; ++i)
    {
      Node::Component* component = objects[i];

      Q_ASSERT(component->hasCustomGlobalCoordUpdater());

      component->_globalCoordFrame = component->calcGlobalCoordFrameImpl();
    }
    break;
  case UpdateType::WithParent:
    // #ISSUE-61 OMP
    for(int i=begin; i!=end; ++i)
    {
      Node::Component* component = objects[i];

      Q_ASSERT(!component->hasCustomGlobalCoordUpdater());
      Q_ASSERT(component->parent != nullptr);

      component->_globalCoordFrame = component->parent->_globalCoordFrame * component->_localCoordFrame;
    }
    break;
  case UpdateType::NoParent:
    // #ISSUE-61 OMP
    for(int i=begin; i!=end; ++i)
    {
      Node::Component* component = objects[i];

      Q_ASSERT(!component->hasCustomGlobalCoordUpdater());
      Q_ASSERT(component->parent == nullptr);

      component->_globalCoordFrame = component->_localCoordFrame;
    }
    break;
  default:
    Q_UNREACHABLE();
  }

  // #ISSUE-61 OMP
  for(int i=begin; i!=end; ++i)
  {
    Node::Component* component = objects[i];

    component->updateZIndex();
  }
}

void GlobalCoordArrayOrder::HasCustomUpdaterHandler::handle_end_segment(Node::Component** objects, int begin, int end, UpdateType updateType, extra_data_type coordManager)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);
  Q_UNUSED(updateType);
  Q_UNUSED(coordManager);
}

// ----

bool GlobalCoordArrayOrder::HasAABBHandler::segmentLessThan(const Node::Component* a, const Node::Component* b)
{
  return segment_as_index(a->hasAABB()) < segment_as_index(b->hasAABB());
}

bool GlobalCoordArrayOrder::HasAABBHandler::classify(const Node::Component* component)
{
  Q_ASSERT(component != nullptr);
  return component->hasAABB();
}

int GlobalCoordArrayOrder::HasAABBHandler::segment_as_index(bool hasAABB)
{
  return hasAABB;
}

bool GlobalCoordArrayOrder::HasAABBHandler::segment_from_index(int i)
{
  return i!=0;
}

void GlobalCoordArrayOrder::HasAABBHandler::handle_new_segment(Node::Component** objects, int begin, int end, bool hasAABB, extra_data_type coordManager)
{
  Q_UNUSED(coordManager);

  Node::Component* component = objects[begin];

  if(Q_LIKELY(hasAABB && component->movable()))
  {
    // parallelization not possible because of `expandSceneAABB`
    for(int i=begin; i!=end; ++i)
    {
      ComponentWithAABB* component = reinterpret_cast<ComponentWithAABB*>(objects[i]);

      // globalAABB is callable here, because HasCustomUpdaterHandler::handle_new_segment has been called before HasAABBHandler::handle_new_segment
      if(component->hasAABB())
        component->expandSceneAABB();
    }
  }
}

void GlobalCoordArrayOrder::HasAABBHandler::handle_end_segment(Node::Component** objects, int begin, int end, bool hasAABB, extra_data_type coordManager)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);
  Q_UNUSED(hasAABB);
  Q_UNUSED(coordManager);
}


} // namespace implementation
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_IMPLEMENTATION_GLOBALCOORDARRAYORDER_INL
