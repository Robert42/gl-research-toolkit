#ifndef GLRT_SCENE_IMPLEMENTATION_GLOBALCOORDARRAYORDER_H
#define GLRT_SCENE_IMPLEMENTATION_GLOBALCOORDARRAYORDER_H

#include <glrt/scene/global-coord-updater.h>

namespace glrt {
namespace scene {

class GlobalCoordUpdater;

namespace implementation {

struct GlobalCoordArrayOrder
{
  enum class UpdateType
  {
    Custom,
    NoParent,
    WithParent
  };

  struct HandlerBase
  {
    typedef GlobalCoordUpdater* extra_data_type;
  };

  struct UpdateCaller : public HandlerBase
  {
    inline static void handle_value(Node::Component**, int, extra_data_type);
    inline static bool valueLessThan(Node::Component* a, Node::Component* b);
  };

  struct DependencyDepthHandler : public HandlerBase
  {
    inline static bool segmentLessThan(const Node::Component* a, const Node::Component* b);
    inline static int classify(const Node::Component* component);
    inline static int segment_as_index(int i);
    inline static int segment_from_index(int i);

    inline static void handle_new_segment(Node::Component** objects, int begin, int end, int depth, extra_data_type coordManager);
    inline static void handle_end_segment(Node::Component** objects, int begin, int end, int depth, extra_data_type coordManager);
  };

  struct HasAABBHandler : public HandlerBase
  {
    inline static bool segmentLessThan(const Node::Component* a, const Node::Component* b);
    inline static bool classify(const Node::Component* component);
    inline static int segment_as_index(bool hasAABB);
    inline static bool segment_from_index(int i);

    inline static void handle_new_segment(Node::Component** objects, int begin, int end, bool hasAABB, extra_data_type coordManager);
    inline static void handle_end_segment(Node::Component** objects, int begin, int end, bool hasAABB, extra_data_type coordManager);
  };

  struct HasCustomUpdaterHandler : public HandlerBase
  {
    inline static bool segmentLessThan(const Node::Component* a, const Node::Component* b);
    inline static UpdateType classify(const Node::Component* component);
    inline static int segment_as_index(UpdateType updateType);
    inline static UpdateType segment_from_index(int i);

    inline static void handle_new_segment(Node::Component** objects, int begin, int end, UpdateType updateType, extra_data_type coordManager);
    inline static void handle_end_segment(Node::Component** objects, int begin, int end, UpdateType updateType, extra_data_type coordManager);
  };

  typedef FragmentedArray_Segment_Values<Node::Component*, UpdateCaller>  CallUpdateTrait;
  typedef FragmentedArray_Segment_Split_in_TwoSegments<Node::Component*, bool, HasAABBHandler, CallUpdateTrait> HasAABBTraits;
  typedef FragmentedArray_Segment_Split_in_FixedNumberOfSegments<3, Node::Component*, UpdateType, HasCustomUpdaterHandler, HasAABBTraits> HasCustomUpdaterTraits;
  typedef FragmentedArray_Segment_Split_in_VariableNumber<Node::Component*, int, DependencyDepthHandler, HasCustomUpdaterTraits> DependencyDepthTraits;

  typedef FragmentedArray<Node::Component*, DependencyDepthTraits> type;
};

} // namespace implementation
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_IMPLEMENTATION_GLOBALCOORDARRAYORDER_H
