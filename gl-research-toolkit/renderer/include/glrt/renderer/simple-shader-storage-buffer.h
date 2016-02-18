#ifndef GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_H
#define GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_H

#include <glrt/renderer/implementation/fragmented-light-component-array.h>
#include <glrt/renderer/synced-fragmented-component-array.h>
#include <glrt/renderer/toolkit/managed-gl-buffer.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glhelper/shaderobject.hpp>

#include <glhelper/buffer.hpp>

namespace glrt {
namespace renderer {

// #ISSUE-64: try out different values for <512, 4096> to find the fastest one
template<class T_LightComponent, typename T_BufferCapacityTraits=ArrayCapacityTraits_Capacity_Blocks<512, 4096>, class T_array_header=implementation::SimpleShaderStorageBuffer_DefaultHeader, int block_to_update=64>
class SimpleShaderStorageBuffer
{
public:
  typedef typename T_LightComponent::Data LightData;

  SimpleShaderStorageBuffer(scene::Scene& scene);

  void update();

  bool needRerecording() const;
  void recordBinding(gl::CommandListRecorder& recorder, GLushort bindingIndex, gl::ShaderObject::ShaderType shaderType);

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
