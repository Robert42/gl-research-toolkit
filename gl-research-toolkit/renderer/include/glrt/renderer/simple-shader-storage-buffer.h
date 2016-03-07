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
template<class T_Component, typename T_FragmentedArray=typename implementation::FragmentedLightComponentArray<T_Component>::type, typename T_BufferCapacityTraits=ArrayCapacityTraits_Capacity_Blocks<512, 4096>>
class SimpleShaderStorageBuffer
{
public:
  typedef typename T_Component::Data LightData;

  SimpleShaderStorageBuffer(scene::Scene& scene);

  void update();

  bool needRerecording() const;
  int numElements() const;
  GLuint64 gpuBufferAddress() const;

private:
  typedef SimpleShaderStorageBuffer<T_Component, T_FragmentedArray, T_BufferCapacityTraits> this_type;
  typedef T_FragmentedArray FragmentedArray;
  typedef SyncedFragmentedComponentArray<T_Component, FragmentedArray> LightComponentArray;

  LightComponentArray lightComponents;
  ManagedGLBuffer<LightData, T_BufferCapacityTraits, implementation::ManagedGLBuffer_NoHeader> buffer;
};

} // namespace renderer
} // namespace glrt

#include "simple-shader-storage-buffer.inl"

#endif // GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_H
