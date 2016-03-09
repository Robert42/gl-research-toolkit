#ifndef GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDLIGHTCOMPONENTARRAY_H
#define GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDLIGHTCOMPONENTARRAY_H

#include "fragmented-component-array.h"
#include <glrt/scene/light-component.h>

namespace glrt {
namespace renderer {
namespace implementation {

template<class T_LightComponent>
struct FragmentedLightComponentArray : public FragmentedComponentArray<T_LightComponent, void*>
{
  typedef FragmentedComponentArray<T_LightComponent, void*> parent_type;
  typedef typename parent_type::MovabilityHintHandler MovabilityHintHandler;
  typedef typename parent_type::MovabilityHint MovabilityHint;
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

  typedef FragmentedArray_Segment_Values<T_LightComponent*, UpdateCaller>  CallUpdateTrait;
  typedef FragmentedArray_Segment_Split_in_FixedNumberOfSegments<3, T_LightComponent*, MovabilityHint, MovabilityHintHandler, CallUpdateTrait> DynamicHintTraits;

  typedef FragmentedArray<T_LightComponent*, DynamicHintTraits> type;
};

} // namespace implementation
} // namespace renderer
} // namespace glrt

#include "fragmented-light-component-array.inl"

#endif // GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDLIGHTCOMPONENTARRAY_H
