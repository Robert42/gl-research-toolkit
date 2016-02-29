#ifndef GLRT_RENDERER_STATICMESHRENDERER_INL
#define GLRT_RENDERER_STATICMESHRENDERER_INL

#include "static-mesh-renderer.h"

namespace glrt {
namespace renderer {


template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::StaticMeshRenderer(scene::Scene& scene)
  : meshComponents(scene)
{
}

template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
void StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::update()
{
}

template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
bool StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::needRerecording() const
{
  return meshComponents.dirty;
}

template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
void StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::recordCommandList(gl::CommandListRecorder& recorder)
{
  T_Recorder staticMeshRecorder(recorder);
  FragmentedArray& fragmentedArray = meshComponents.fragmented_array;

  meshComponents.dirty = false;

  fragmentedArray.updateSegments(&staticMeshRecorder);
  fragmentedArray.iterate(&staticMeshRecorder);
}


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESHRENDERER_INL
