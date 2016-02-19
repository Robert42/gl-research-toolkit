#ifndef GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_INL
#define GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_INL

#include "simple-shader-storage-buffer.h"

namespace glrt {
namespace renderer {

template<class T_Component, typename T_FragmentedArray, typename T_BufferCapacityTraits, class T_array_header, int block_to_update>
SimpleShaderStorageBuffer<T_Component,T_FragmentedArray,T_BufferCapacityTraits,T_array_header, block_to_update>::SimpleShaderStorageBuffer(scene::Scene& scene)
  : lightComponents(scene)
{
}


template<class T_Component, typename T_FragmentedArray, typename T_BufferCapacityTraits, class T_array_header, int block_to_update>
void SimpleShaderStorageBuffer<T_Component,T_FragmentedArray,T_BufferCapacityTraits,T_array_header, block_to_update>::update()
{
  FragmentedArray& fragmented_array = lightComponents.fragmented_array;
  const int indexOfFirstUpdated = fragmented_array.updateSegments(nullptr);
  const int length = fragmented_array.length();

  buffer.markElementDirty(indexOfFirstUpdated);
  buffer.setNumElements(length);

  if(buffer.needsUpdate())
  {
    T_Component** src = fragmented_array.data();
    LightData* dest = buffer.Map();

    // ISSUE-61 OMP
    for(int i=buffer.firstElementToCopy(); i<length; ++i)
      dest[i] = src[i]->globalData();

    buffer.Unmap();
  }
}

template<class T_Component, typename T_FragmentedArray, typename T_BufferCapacityTraits, class T_array_header, int block_to_update>
bool SimpleShaderStorageBuffer<T_Component,T_FragmentedArray,T_BufferCapacityTraits,T_array_header, block_to_update>::needRerecording() const
{
  return buffer.gpuAddressChanged;
}

template<class T_Component, typename T_FragmentedArray, typename T_BufferCapacityTraits, class T_array_header, int block_to_update>
void SimpleShaderStorageBuffer<T_Component,T_FragmentedArray,T_BufferCapacityTraits,T_array_header, block_to_update>::recordBinding(gl::CommandListRecorder& recorder, GLushort bindingIndex, gl::ShaderObject::ShaderType shaderType)
{
  recorder.append_token_UniformAddress(bindingIndex, shaderType, buffer.buffer.gpuBufferAddress());
  buffer.gpuAddressChanged = false;
}


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_HINL
