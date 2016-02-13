#ifndef GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDLIGHTCOMPONENTARRAY_INL
#define GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDLIGHTCOMPONENTARRAY_INL

#include "fragmented-light-component-array.h"

namespace glrt {
namespace renderer {
namespace implementation {

template<class T_LightComponent>
void FragmentedLightComponentArray<T_LightComponent>::UpdateCaller::handle_value(T_LightComponent**, int, extra_data_type)
{
}

template<class T_LightComponent>
bool FragmentedLightComponentArray<T_LightComponent>::UpdateCaller::valueLessThan(T_LightComponent* a, T_LightComponent* b)
{
  return a < b;
}

template<class T_LightComponent>
bool FragmentedLightComponentArray<T_LightComponent>::DynamicHintHandler::segmentLessThan(const T_LightComponent* a, const T_LightComponent* b)
{
  return classify(a) < classify(b);
}

template<class T_LightComponent>
int FragmentedLightComponentArray<T_LightComponent>::DynamicHintHandler::classify(const T_LightComponent* component)
{
  // There are three possibilities: 0b00 for not movable and not becoming movable
  // There are three possibilities: 0b01 for not movable and may becoming movable
  // There are three possibilities: 0b11 for not movable

  int movable = static_cast<int>(component->movable());
  int mayBecomeMovable = static_cast<int>(component->movable());
  Q_ASSERT(movable<=1);
  Q_ASSERT(mayBecomeMovable<=1);
  return (movable<<1) | (mayBecomeMovable | movable);
}

template<class T_LightComponent>
int FragmentedLightComponentArray<T_LightComponent>::DynamicHintHandler::segment_as_index(int i)
{
  return i;
}

template<class T_LightComponent>
int FragmentedLightComponentArray<T_LightComponent>::DynamicHintHandler::segment_from_index(int i)
{
  return i;
}

template<class T_LightComponent>
void FragmentedLightComponentArray<T_LightComponent>::DynamicHintHandler::handle_new_segment(T_LightComponent** objects, int begin, int end, int depth, extra_data_type)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);
  Q_UNUSED(depth);
}

template<class T_LightComponent>
void FragmentedLightComponentArray<T_LightComponent>::DynamicHintHandler::handle_end_segment(T_LightComponent** objects, int begin, int end, int depth, extra_data_type)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);
  Q_UNUSED(depth);
}

} // namespace implementation
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDLIGHTCOMPONENTARRAY_INL
