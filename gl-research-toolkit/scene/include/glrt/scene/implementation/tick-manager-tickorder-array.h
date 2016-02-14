#ifndef GLRT_SCENE_IMPLEMENTATION_TICKMANAGER_TICKORDER_H
#define GLRT_SCENE_IMPLEMENTATION_TICKMANAGER_TICKORDER_H

#include <glrt/scene/tick-manager.h>

namespace glrt {
namespace scene {
namespace implementation {

struct TickManager_TickObjectArray
{
  struct HandlerBase
  {
    typedef TickManager* extra_data_type;
  };

  struct TickCaller : public HandlerBase
  {
    inline static void handle_value(TickingObject**, int, TickManager*);
    inline static bool valueLessThan(TickingObject* a, TickingObject* b);
  };

  struct DependencyDepthHandler : public HandlerBase
  {
    inline static bool segmentLessThan(const TickingObject* a, const TickingObject* b);
    inline static int classify(const TickingObject* tickingObject);
    inline static int segment_as_index(int i);
    inline static int segment_from_index(int i);

    inline static void handle_new_segment(TickingObject** objects, int begin, int end, int depth, TickManager* tickManager);
    inline static void handle_end_segment(TickingObject** objects, int begin, int end, int depth, TickManager* tickManager);
  };

  struct MultithreadingHandler : public HandlerBase
  {
    inline static bool segmentLessThan(const TickingObject* a, const TickingObject* b);
    inline static TickingObject::TickTraits classify(const TickingObject* tickingObject);

    inline static int segment_as_index(TickingObject::TickTraits i);
    inline static TickingObject::TickTraits segment_from_index(int i);
    inline static void handle_new_segment(TickingObject** objects, int begin, int end, TickingObject::TickTraits tickTraits, TickManager* tickManager);
    inline static void handle_end_segment(TickingObject** objects, int begin, int end, TickingObject::TickTraits tickTraits, TickManager* tickManager);
  };

  typedef FragmentedArray_Segment_Values<TickingObject*, TickCaller>  CallTickTrait;
  typedef FragmentedArray_Segment_Split_in_TwoSegments<TickingObject*, TickingObject::TickTraits, MultithreadingHandler, CallTickTrait> MultithreadingTraits;
  typedef FragmentedArray_Segment_Split_in_VariableNumber<TickingObject*, int, DependencyDepthHandler, MultithreadingTraits> DependencyDepthTraits;

  typedef FragmentedArray<TickingObject*, DependencyDepthTraits> type;
};

} // namespace implementation
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_IMPLEMENTATION_TICKMANAGER_TICKORDER_H
