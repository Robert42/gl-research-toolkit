#ifndef GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_INL
#define GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_INL

#include "simple-shader-storage-buffer.h"

namespace glrt {
namespace renderer {

template<class T_LightComponent, class T_array_header, int block_to_update>
SimpleShaderStorageBuffer<T_LightComponent,T_array_header, block_to_update>::SimpleShaderStorageBuffer(scene::Scene& scene)
  : lightComponents(scene),
    buffer(data_array.capacity() + sizeof(T_array_header), gl::Buffer::MAP_WRITE)
{
}



template<class T_LightComponent, class T_array_header, int block_to_update>
struct SimpleShaderStorageBuffer<T_LightComponent,T_array_header, block_to_update>::Updater
{
  typedef quint8 byte;

  static const int header_size = sizeof(T_array_header);

  this_type& shaderStorageBuffer;
  FragmentedArray& fragmented_array;

  const int total_number_of_components;
  const int total_number_of_bytes;
  const int indexOfFirstMovableComponent;
  const int indexOfFirstUpdated;
  int firstByteToCopy;

  T_array_header* header = nullptr;
  const byte* wholeBuffer = nullptr;

  bool numberOfComponentsChanged = false;

  Updater(this_type* shaderStorageBuffer, int indexOfFirstUpdated)
    : shaderStorageBuffer(*shaderStorageBuffer),
      fragmented_array(shaderStorageBuffer->lightComponents.fragmented_array),
      total_number_of_components(fragmented_array.length()),
      total_number_of_bytes(total_number_of_components*sizeof(LightData) + header_size),
      indexOfFirstMovableComponent(fragmented_array.section_boundaries(implementation::FragmentedLightComponentArray<T_LightComponent>::firstSegmentWithMovable()).x),
      indexOfFirstUpdated(indexOfFirstUpdated)
  {
    Q_ASSERT(indexOfFirstMovableComponent>=0);
    Q_ASSERT(indexOfFirstMovableComponent<total_number_of_components);

    firstByteToCopy = glm::min(indexOfFirstUpdated, indexOfFirstMovableComponent) * sizeof(LightData) + header_size;
  }

  void update_capacity()
  {
    if(total_number_of_components != shaderStorageBuffer.data_array.length())
    {
      numberOfComponentsChanged = true;

      // #TODO: performance bottleneck? There's no meaning in copying values which will be replaces anyway
      // #FIXME shaderStorageBuffer.data_array.resize(total_number_of_components);

      setup_pointers();
      header->numElements = total_number_of_components;

      if(firstByteToCopy < block_to_update)
      {
        firstByteToCopy = 0;
      }else
      {
        void* bufferData = shaderStorageBuffer.buffer.Map(0, header_size, gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_RANGE);
        std::memcpy(bufferData, &header, sizeof(T_array_header));
        shaderStorageBuffer.buffer.Unmap();
      }
    }
  }

  void setup_pointers()
  {
    header = &DataArrayAllocator::prepended_data(shaderStorageBuffer.data_array.data());

    wholeBuffer = reinterpret_cast<byte*>(header);
  }

  void update_array_content()
  {
    const int begin=glm::min(indexOfFirstMovableComponent, indexOfFirstUpdated);
    const int end=total_number_of_components;
    LightData* dest = shaderStorageBuffer.data_array.data();
    T_LightComponent** src = fragmented_array.data();

    // ISSUE-61 OMP
    for(int i=begin; i<end; ++i)
    {
      dest[i] = src[i]->data;
    }
  }

  bool needToAllocNewGpuBuffer() const
  {
    return shaderStorageBuffer.data_array.capacity() + sizeof(T_array_header) != shaderStorageBuffer.buffer.GetSize();
  }

  void createGpuBuffer()
  {
    shaderStorageBuffer.buffer = std::move(gl::Buffer(shaderStorageBuffer.data_array.capacity()*sizeof(LightData) + sizeof(T_array_header), gl::Buffer::MAP_WRITE));
  }

  void copyWholeBuffer()
  {
    void* bufferData = shaderStorageBuffer.buffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER);
    std::memcpy(bufferData, shaderStorageBuffer.data_array.data(), static_cast<size_t>(total_number_of_bytes));
    shaderStorageBuffer.buffer.Unmap();
  }

  void copyDirtyPartOfBuffer()
  {
    int totalBytesToCopy = total_number_of_bytes-firstByteToCopy;

    void* bufferData = shaderStorageBuffer.buffer.Map(firstByteToCopy, totalBytesToCopy, gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER);
    std::memcpy(bufferData, wholeBuffer+firstByteToCopy, static_cast<size_t>(totalBytesToCopy));
    shaderStorageBuffer.buffer.Unmap();
  }
};


template<class T_LightComponent, class T_array_header, int block_to_update>
void SimpleShaderStorageBuffer<T_LightComponent,T_array_header, block_to_update>::update()
{
  int indexOfFirstUpdated = lightComponents.fragmented_array.updateSegments(nullptr);

  Updater updater(this, indexOfFirstUpdated);
  updater.setup_pointers();
  updater.update_capacity();
  updater.update_array_content();

  if(updater.needToAllocNewGpuBuffer())
  {
    updater.createGpuBuffer();
    updater.copyWholeBuffer();
  }else if(lightComponents.dirty)
  {
    updater.copyDirtyPartOfBuffer();
  }

  lightComponents.dirty = false;
}


template<class T_LightComponent, class T_array_header, int block_to_update>
void SimpleShaderStorageBuffer<T_LightComponent,T_array_header, block_to_update>::bindShaderStorageBuffer(int bindingIndex)
{
  buffer.BindShaderStorageBuffer(bindingIndex);
}


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_HINL
