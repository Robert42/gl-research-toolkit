#ifndef GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDSTATICMESHCOMPONENTARRAY_INL
#define GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDSTATICMESHCOMPONENTARRAY_INL

#include "fragmented-static-mesh-component-array.h"

#include <glrt/scene/static-mesh-component.h>
#include <glrt/toolkit/concatenated-less-than.h>

namespace glrt {
namespace renderer {
namespace implementation {

template<class T_StaticMeshComponent, class T_recorder>
void FragmentedStaticMeshComponentArray<T_StaticMeshComponent, T_recorder>::UpdateCaller::handle_value(T_StaticMeshComponent** components, int index, recorder_type* recorder)
{
  Q_UNUSED(components);
  Q_UNUSED(index);
  Q_UNUSED(recorder);
}

template<class T_StaticMeshComponent, class T_recorder>
bool FragmentedStaticMeshComponentArray<T_StaticMeshComponent, T_recorder>::UpdateCaller::valueLessThan(T_StaticMeshComponent* a, T_StaticMeshComponent* b)
{
  return a < b;
}


template<class T_StaticMeshComponent, class T_recorder>
bool FragmentedStaticMeshComponentArray<T_StaticMeshComponent, T_recorder>::MaterialHandler::segmentLessThan(const T_StaticMeshComponent* a, const T_StaticMeshComponent* b)
{
  return concatenated_lessThan(a->material(), b->material(), a->materialUuid, b->materialUuid);
}

template<class T_StaticMeshComponent, class T_recorder>
Uuid<glrt::scene::resources::Material> FragmentedStaticMeshComponentArray<T_StaticMeshComponent, T_recorder>::MaterialHandler::classify(const T_StaticMeshComponent* component)
{
  return component->materialUuid;
}

template<class T_StaticMeshComponent, class T_recorder>
void FragmentedStaticMeshComponentArray<T_StaticMeshComponent, T_recorder>::MaterialHandler::handle_new_segment(T_StaticMeshComponent** objects, int begin, int end, Uuid<Material> material, recorder_type* recorder)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);

  recorder->bindMaterial(material);
}

template<class T_StaticMeshComponent, class T_recorder>
void FragmentedStaticMeshComponentArray<T_StaticMeshComponent, T_recorder>::MaterialHandler::handle_end_segment(T_StaticMeshComponent** objects, int begin, int end, Uuid<Material> material, recorder_type* recorder)
{
  Q_UNUSED(objects);
  Q_UNUSED(begin);
  Q_UNUSED(end);

  recorder->unbindMaterial(material);
}


} // namespace implementation
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_IMPLEMENTATION_FRAGMENTEDSTATICMESHCOMPONENTARRAY_INL
