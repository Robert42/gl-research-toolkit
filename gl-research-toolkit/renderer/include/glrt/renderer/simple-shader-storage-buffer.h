#ifndef GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_H
#define GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_H

#include <glrt/renderer/implementation/fragmented-light-component-array.h>
#include <glrt/renderer/synced-fragmented-component-array.h>
#include <glrt/renderer/toolkit/managed-gl-buffer.h>

#include <glhelper/buffer.hpp>

namespace glrt {
namespace renderer {

// #TODO: is there a way to get the best value for block_to_update from opengl? (Its the size which is send together as a block to opengl) or find out experimentally which block_to_update value is the fastest
template<class T_LightComponent, typename T_BufferCapacityTraits=ArrayCapacityTraits_Capacity_Blocks<512, 4096>, class T_array_header=implementation::SimpleShaderStorageBuffer_DefaultHeader, int block_to_update=64>
class SimpleShaderStorageBuffer
{
public:
  typedef typename T_LightComponent::Data LightData;


  SimpleShaderStorageBuffer(scene::Scene& scene);

  void update();

  void bindShaderStorageBuffer(int bindingIndex);

private:
  typedef SimpleShaderStorageBuffer<T_LightComponent, T_BufferCapacityTraits,T_array_header, block_to_update> this_type;
  typedef typename implementation::FragmentedLightComponentArray<T_LightComponent>::type FragmentedArray;
  typedef SyncedFragmentedComponentArray<T_LightComponent, FragmentedArray> LightComponentArray;

  LightComponentArray lightComponents;
  ManagedGLBuffer<LightData, T_BufferCapacityTraits, implementation::ManagedGLBuffer_Header_With_Num_Elements<T_array_header>> buffer;
};

} // namespace renderer
} // namespace glrt

#include "simple-shader-storage-buffer.inl"

#endif // GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_H
