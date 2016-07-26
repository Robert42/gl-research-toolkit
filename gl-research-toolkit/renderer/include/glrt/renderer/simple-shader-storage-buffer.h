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
namespace implementation {

template<typename T_Component>
struct DefaultDataDescription
{
  typedef typename T_Component::Data data_type;

  static data_type data_from_component(const T_Component* component);
};

template<typename T_Component, typename T_Data, T_Data(T_Component::*get_data_ptr)() const>
struct RandomComponentDataDescription
{
  typedef T_Data data_type;

  static data_type data_from_component(const T_Component* component);
};

} // namespace


// #ISSUE-64: try out different values for <512, 4096> to find the fastest one
template<class T_Component, typename T_DataDescription=implementation::DefaultDataDescription<T_Component>, typename T_FragmentedArray=typename implementation::FragmentedLightComponentArray<T_Component>::type, typename T_BufferCapacityTraits=ArrayCapacityTraits_Capacity_Blocks<512, 4096>>
class SimpleShaderStorageBuffer
{
public:
  typedef typename T_DataDescription::data_type data_type;

  SimpleShaderStorageBuffer(scene::Scene& scene);

  void update();

  bool needRerecording() const;
  int numElements() const;
  const T_Component* const * data() const;
  GLuint64 gpuBufferAddress() const;

private:
  typedef SimpleShaderStorageBuffer<T_Component, T_FragmentedArray, T_BufferCapacityTraits> this_type;
  typedef T_FragmentedArray FragmentedArray;
  typedef SyncedFragmentedComponentArray<T_Component, FragmentedArray> LightComponentArray;

  LightComponentArray lightComponents;
  ManagedGLBuffer<data_type, T_BufferCapacityTraits, implementation::ManagedGLBuffer_NoHeader> buffer;
};

} // namespace renderer
} // namespace glrt

#include "simple-shader-storage-buffer.inl"

#endif // GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_H
