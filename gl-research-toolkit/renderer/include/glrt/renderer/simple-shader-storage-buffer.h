#ifndef GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_H
#define GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_H

#include <glrt/renderer/implementation/fragmented-light-component-array.h>
#include <glrt/renderer/synced-fragmented-component-array.h>

#include <glhelper/buffer.hpp>

namespace glrt {
namespace renderer {
namespace implementation {

struct SimpleShaderStorageBuffer_DefaultHeader
{
  int numElements;
  padding<int, 3> _padding;
};

} // namespace implementation

// #TODO: is there a way to get the best value for block_to_update from opengl? (Its the size which is send together as a block to opengl) or find out experimentally which block_to_update value is the fastest
template<class T_LightComponent, class T_array_header=implementation::SimpleShaderStorageBuffer_DefaultHeader, int block_to_update=64>
class SimpleShaderStorageBuffer
{
public:
  typedef typename T_LightComponent::Data LightData;
  typedef AllocatorWithPrependedData<LightData, T_array_header> DataArrayAllocator;
  Array<LightData, typename DefaultTraits<LightData>::type, DataArrayAllocator > data_array;

  SimpleShaderStorageBuffer(scene::Scene& scene);

  void update();

  void bindShaderStorageBuffer(int bindingIndex);

private:
  typedef SyncedFragmentedComponentArray<T_LightComponent, typename implementation::FragmentedLightComponentArray<T_LightComponent>::type> LightComponentArray;

  LightComponentArray lightComponents;
  gl::Buffer buffer;
};

} // namespace renderer
} // namespace glrt

#include "simple-shader-storage-buffer.inl"

#endif // GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_H
