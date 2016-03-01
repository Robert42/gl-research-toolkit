#ifndef GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDSTATICMESHCOMPONENTARRAY_H
#define GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDSTATICMESHCOMPONENTARRAY_H

#include "fragmented-component-array.h"

namespace glrt {
namespace renderer {
namespace implementation {

typedef typename glrt::scene::resources::Material Material;

bool materialLessThan(const Material& a, const Material& b, const Uuid<Material>& uuidA, const Uuid<Material>& uuidB);

template<class T_StaticMeshComponent, class T_recorder>
struct FragmentedStaticMeshComponentArray : public FragmentedComponentArray<T_StaticMeshComponent, T_recorder*>
{
  typedef T_recorder recorder_type;
  typedef FragmentedStaticMeshComponentArray<T_StaticMeshComponent, T_recorder> this_type;
  typedef FragmentedComponentArray<T_StaticMeshComponent, recorder_type*> parent_type;
  typedef typename parent_type::MovabilityHintHandler MovabilityHintHandler;
  typedef typename parent_type::MovabilityHint MovabilityHint;

  struct HandlerBase
  {
    typedef T_recorder* extra_data_type;
  };

  struct UpdateCaller : public HandlerBase
  {
    inline static void handle_value(T_StaticMeshComponent**, int, recorder_type*);
    inline static bool valueLessThan(T_StaticMeshComponent* a, T_StaticMeshComponent* b);
  };

  struct MaterialHandler : public HandlerBase
  {
    inline static bool segmentLessThan(const T_StaticMeshComponent* a, const T_StaticMeshComponent* b);
    inline static Uuid<Material> classify(const T_StaticMeshComponent* component);

    inline static void handle_new_segment(T_StaticMeshComponent** objects, int begin, int end, Uuid<Material> material, recorder_type*);
    inline static void handle_end_segment(T_StaticMeshComponent** objects, int begin, int end, Uuid<Material> material, recorder_type*);
  };

  typedef FragmentedArray_Segment_Values<T_StaticMeshComponent*, UpdateCaller>  CallUpdateTrait;
  typedef FragmentedArray_Segment_Generic<T_StaticMeshComponent*, Uuid<Material>, MaterialHandler, CallUpdateTrait> MaterialHandlerTrait;
  typedef FragmentedArray_Segment_Split_in_FixedNumberOfSegments<3, T_StaticMeshComponent*, MovabilityHint, MovabilityHintHandler, MaterialHandlerTrait> DynamicHintTraits;

  typedef FragmentedArray<T_StaticMeshComponent*, DynamicHintTraits> type;
};

} // namespace implementation
} // namespace renderer
} // namespace glrt

#include "fragmented-static-mesh-component-array.inl"

#endif // GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDSTATICMESHCOMPONENTARRAY_H
