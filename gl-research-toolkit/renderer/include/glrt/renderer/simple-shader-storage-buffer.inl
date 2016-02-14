#ifndef GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_INL
#define GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_INL

#include "simple-shader-storage-buffer.h"

namespace glrt {
namespace renderer {

template<class T_LightComponent, typename T_BufferCapacityTraits, class T_array_header, int block_to_update>
SimpleShaderStorageBuffer<T_LightComponent,T_BufferCapacityTraits,T_array_header, block_to_update>::SimpleShaderStorageBuffer(scene::Scene& scene)
  : lightComponents(scene)
{
}


template<class T_LightComponent, typename T_BufferCapacityTraits, class T_array_header, int block_to_update>
void SimpleShaderStorageBuffer<T_LightComponent,T_BufferCapacityTraits,T_array_header, block_to_update>::update()
{
  FragmentedArray& fragmented_array = lightComponents.fragmented_array;
  const int indexOfFirstUpdated = fragmented_array.updateSegments(nullptr);
  const int length = fragmented_array.length();

  buffer.markElementDirty(indexOfFirstUpdated);
  buffer.setNumElements(length);

  if(buffer.needsUpdate())
  {
    T_LightComponent** src = fragmented_array.data();
    LightData* dest = buffer.Map();

    // ISSUE-61 OMP
    for(int i=indexOfFirstUpdated; i<length; ++i)
      dest[i] = src[i]->data;

    buffer.Unmap();
  }
}

template<class T_LightComponent, typename T_BufferCapacityTraits, class T_array_header, int block_to_update>
void SimpleShaderStorageBuffer<T_LightComponent,T_BufferCapacityTraits,T_array_header, block_to_update>::bindShaderStorageBuffer(int bindingIndex)
{
  buffer.buffer.BindShaderStorageBuffer(bindingIndex);
}


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_HINL
