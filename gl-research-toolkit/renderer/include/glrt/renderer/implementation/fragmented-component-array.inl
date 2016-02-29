#ifndef GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDCOMPONENTARRAY_INL
#define GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDCOMPONENTARRAY_INL

#include "fragmented-component-array.h"

namespace glrt {
namespace renderer {
namespace implementation {

template<class T_Component, typename T_extra_data_type>
bool FragmentedComponentArray<T_Component, T_extra_data_type>::MovabilityHintHandler::segmentLessThan(const T_Component* a, const T_Component* b)
{
  return classify(a) < classify(b);
}

template<class T_Component, typename T_extra_data_type>
glrt::scene::Node::Component::MovabilityHint FragmentedComponentArray<T_Component, T_extra_data_type>::MovabilityHintHandler::classify(const T_Component* component)
{
  return component->movabilityHint();
}

template<class T_Component, typename T_extra_data_type>
int FragmentedComponentArray<T_Component, T_extra_data_type>::MovabilityHintHandler::segment_as_index(MovabilityHint i)
{
  return static_cast<int>(i);
}

template<class T_Component, typename T_extra_data_type>
glrt::scene::Node::Component::MovabilityHint FragmentedComponentArray<T_Component, T_extra_data_type>::MovabilityHintHandler::segment_from_index(int i)
{
  return static_cast<MovabilityHint>(i);
}

template<class T_Component, typename T_extra_data_type>
void FragmentedComponentArray<T_Component, T_extra_data_type>::MovabilityHintHandler::handle_new_segment(T_Component** objects, int begin, int end, MovabilityHint hint, extra_data_type)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);
  Q_UNUSED(hint);
}

template<class T_Component, typename T_extra_data_type>
void FragmentedComponentArray<T_Component, T_extra_data_type>::MovabilityHintHandler::handle_end_segment(T_Component** objects, int begin, int end, MovabilityHint hint, T_extra_data_type)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);
  Q_UNUSED(hint);
}

} // namespace implementation
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDCOMPONENTARRAY_INL
