#ifndef GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDCOMPONENTARRAY_H
#define GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDCOMPONENTARRAY_H

#include <glrt/toolkit/fragmented-array.h>
#include <glrt/toolkit/concatenated-less-than.h>
#include <glrt/scene/node.h>

namespace glrt {
namespace renderer {
namespace implementation {

template<class T_Component, typename T_extra_data_type>
struct FragmentedComponentArray
{
  typedef glrt::scene::Node::Component::MovabilityHint MovabilityHint;
  typedef T_extra_data_type extra_data_type;

  struct HandlerBase
  {
    typedef T_extra_data_type extra_data_type;
  };

  struct MovabilityHintHandler : public HandlerBase
  {
    inline static bool segmentLessThan(const T_Component* a, const T_Component* b);
    inline static MovabilityHint classify(const T_Component* component);
    inline static int segment_as_index(MovabilityHint i);
    inline static MovabilityHint segment_from_index(int i);

    inline static void handle_new_segment(T_Component** objects, int begin, int end, MovabilityHint hint, extra_data_type);
    inline static void handle_end_segment(T_Component** objects, int begin, int end, MovabilityHint hint, extra_data_type);
  };
};

} // namespace implementation
} // namespace renderer
} // namespace glrt

#include "fragmented-component-array.inl"

#endif // GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDCOMPONENTARRAY_H
