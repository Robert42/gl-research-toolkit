#ifndef GLRT_SCENE_IMPLEMENTATION_GLOBALCOORDARRAYORDER_INL
#define GLRT_SCENE_IMPLEMENTATION_GLOBALCOORDARRAYORDER_INL

#include <glrt/scene/global-coord-updater.h>

namespace glrt {
namespace scene {
namespace implementation {

void GlobalCoordArrayOrder::UpdateCaller::handle_value(Node::Component**, int, extra_data_type)
{
}

bool GlobalCoordArrayOrder::UpdateCaller::valueLessThan(Node::Component* a, Node::Component* b)
{
  return a<b;
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

void GlobalCoordArrayOrder::DependencyDepthHandler::handle_new_segment(Node::Component**, int, int, int, extra_data_type)
{
}

void GlobalCoordArrayOrder::DependencyDepthHandler::handle_end_segment(Node::Component**, int, int, int, extra_data_type)
{
}


bool GlobalCoordArrayOrder::HasCustomUpdaterHandler::segmentLessThan(const Node::Component* a, const Node::Component* b)
{
  return segment_as_index(a->hasCustomGlobalCoordUpdater()) < segment_as_index(b->hasCustomGlobalCoordUpdater());
}

bool GlobalCoordArrayOrder::HasCustomUpdaterHandler::classify(const Node::Component* component)
{
  Q_ASSERT(component != nullptr);
  return component->hasCustomGlobalCoordUpdater();
}

int GlobalCoordArrayOrder::HasCustomUpdaterHandler::segment_as_index(bool hasCustomUpdater)
{
  return hasCustomUpdater;
}

bool GlobalCoordArrayOrder::HasCustomUpdaterHandler::segment_from_index(int i)
{
  return i!=0;
}

void GlobalCoordArrayOrder::HasCustomUpdaterHandler::handle_new_segment(Node::Component** objects, int begin, int end, bool hasCustomUpdater, extra_data_type coordManager)
{
  Q_UNUSED(coordManager);

  if(hasCustomUpdater)
  {
    // #ISSUE-61 OMP
    for(int i=begin; i!=end; ++i)
    {
      Node::Component* component = objects[i];

      component->_globalCoordFrame = component->calcGlobalCoordFrameImpl();
    }
  }else
  {
    // #ISSUE-61 OMP
    for(int i=begin; i!=end; ++i)
    {
      Node::Component* component = objects[i];

      component->_globalCoordFrame = component->parent->_globalCoordFrame * component->_localCoordFrame;
    }
  }
}

void GlobalCoordArrayOrder::HasCustomUpdaterHandler::handle_end_segment(Node::Component** objects, int begin, int end, bool hasCustomUpdater, extra_data_type coordManager)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);
  Q_UNUSED(hasCustomUpdater);
  Q_UNUSED(coordManager);
}


} // namespace implementation
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_IMPLEMENTATION_GLOBALCOORDARRAYORDER_INL
