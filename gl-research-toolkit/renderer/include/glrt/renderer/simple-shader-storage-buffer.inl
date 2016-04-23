#ifndef GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_INL
#define GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_INL

#include "simple-shader-storage-buffer.h"

namespace glrt {
namespace renderer {
namespace implementation {

template<typename T_Component>
typename DefaultDataDescription<T_Component>::data_type DefaultDataDescription<T_Component>::data_from_component(const T_Component* component)
{
  return component->globalData();
}

} // namespace renderer

template<class T_Component, typename T_DataDescription, typename T_FragmentedArray, typename T_BufferCapacityTraits>
SimpleShaderStorageBuffer<T_Component,T_DataDescription,T_FragmentedArray,T_BufferCapacityTraits>::SimpleShaderStorageBuffer(scene::Scene& scene)
  : lightComponents(scene)
{
}


template<class T_Component, typename T_DataDescription, typename T_FragmentedArray, typename T_BufferCapacityTraits>
void SimpleShaderStorageBuffer<T_Component,T_DataDescription,T_FragmentedArray,T_BufferCapacityTraits>::update()
{
  FragmentedArray& fragmented_array = lightComponents.fragmented_array;
  const int indexOfFirstUpdated = fragmented_array.updateSegments(nullptr);
  const int length = fragmented_array.length();

  buffer.markElementDirty(indexOfFirstUpdated);
  buffer.setNumElements(length);

  if(buffer.needsUpdate())
  {
    T_Component** src = fragmented_array.data();
    data_type* dest = buffer.Map();

    // ISSUE-61 OMP
    for(int i=buffer.firstElementToCopy(); i<length; ++i)
      dest[i] = T_DataDescription::data_from_component(src[i]);

    buffer.Unmap();
  }
}

template<class T_Component, typename T_DataDescription, typename T_FragmentedArray, typename T_BufferCapacityTraits>
bool SimpleShaderStorageBuffer<T_Component,T_DataDescription,T_FragmentedArray,T_BufferCapacityTraits>::needRerecording() const
{
  return buffer.gpuAddressChanged;
}

template<class T_Component, typename T_DataDescription, typename T_FragmentedArray, typename T_BufferCapacityTraits>
int SimpleShaderStorageBuffer<T_Component,T_DataDescription,T_FragmentedArray,T_BufferCapacityTraits>::numElements() const
{
  return buffer.numElements();
}

template<class T_Component, typename T_DataDescription, typename T_FragmentedArray, typename T_BufferCapacityTraits>
GLuint64 SimpleShaderStorageBuffer<T_Component,T_DataDescription,T_FragmentedArray,T_BufferCapacityTraits>::gpuBufferAddress() const
{
  return buffer.buffer.gpuBufferAddress();
}


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_HINL
