#ifndef GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDLIGHTCOMPONENTARRAY_H
#define GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDLIGHTCOMPONENTARRAY_H

#include <glrt/toolkit/fragmented-array.h>
#include <glrt/toolkit/concatenated-less-than.h>
#include <glrt/scene/light-component.h>

namespace glrt {
namespace renderer {
namespace implementation {

template<class T_LightComponent>
struct FragmentedLightComponentArray
{
  typedef void* extra_data_type;

  struct HandlerBase
  {
    typedef void* extra_data_type;
  };

  struct UpdateCaller : public HandlerBase
  {
    inline static void handle_value(T_LightComponent**, int, extra_data_type);
    inline static bool valueLessThan(T_LightComponent* a, T_LightComponent* b);
  };

  struct DynamicHintHandler : public HandlerBase
  {
    inline static bool segmentLessThan(const T_LightComponent* a, const T_LightComponent* b);
    inline static int classify(const T_LightComponent* component);
    inline static int segment_as_index(int i);
    inline static int segment_from_index(int i);

    inline static void handle_new_segment(T_LightComponent** objects, int begin, int end, int depth, extra_data_type);
    inline static void handle_end_segment(T_LightComponent** objects, int begin, int end, int depth, extra_data_type);
  };

  typedef FragmentedArray_Segment_Values<T_LightComponent*, UpdateCaller>  CallUpdateTrait;
  typedef FragmentedArray_Segment_Split_in_FixedNumberOfSegments<3, T_LightComponent*, int, DynamicHintHandler, CallUpdateTrait> DynamicHintTraits;

  typedef FragmentedArray<T_LightComponent*, DynamicHintTraits> type;
};

} // namespace implementation
} // namespace renderer
} // namespace glrt

#include "fragmented-light-component-array.inl"

#endif // GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDLIGHTCOMPONENTARRAY_H
