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
  return a->zIndex() < b->zIndex();
}


} // namespace implementation
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDLIGHTCOMPONENTARRAY_INL
