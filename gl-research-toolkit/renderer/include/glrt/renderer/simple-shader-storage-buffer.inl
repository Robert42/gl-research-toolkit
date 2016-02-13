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
void SimpleShaderStorageBuffer<T_LightComponent,T_array_header, block_to_update>::update()
{
  int firstMovable = lightComponents.fragmented_array.section_boundaries(implementation::FragmentedLightComponentArray<T_LightComponent>::firstSegmentWithMovable()).x;
  int dataChanged = firstMovable;

  const void* pointerToHeaderStart = &DataArrayAllocator::prepended_data(data_array.data());
  const void* dataToWrite = data_array.data();
  int firstByteToWrite = dataChanged*sizeof(LightData);
  GLsizeiptr numberOfBytesToWrite = (data_array.length()-dataChanged) * sizeof(LightData);
  gl::Buffer::MapWriteFlag writeFlag = gl::Buffer::MapWriteFlag::INVALIDATE_RANGE;

  data_array.reserve(lightComponents.fragmented_array.length());
  if(data_array.capacity() + sizeof(T_array_header) != buffer.GetSize())
    buffer = std::move(gl::Buffer(data_array.capacity() + sizeof(T_array_header), gl::Buffer::MAP_WRITE));

  if(lightComponents.dirty)
  {
    int& numElements = DataArrayAllocator::prepended_data(data_array.data()).numElements;

    bool needToUpdateNumElements = false;
    if(numElements != lightComponents.fragmented_array.length())
    {
      numElements = lightComponents.fragmented_array.length();
      needToUpdateNumElements = true;
    }

    dataChanged = lightComponents.fragmented_array.updateSegments(nullptr);
    dataChanged = glm::min<int>(dataChanged, firstMovable);

    firstByteToWrite = dataChanged*sizeof(LightData);
    numberOfBytesToWrite = (data_array.length()-dataChanged) * sizeof(LightData);

    if(needToUpdateNumElements)
    {
      if(dataChanged <= block_to_update)
      {
        firstByteToWrite = 0;
        numberOfBytesToWrite = sizeof(LightData)*data_array.length() + sizeof(T_array_header);
        dataToWrite = pointerToHeaderStart;
        writeFlag = gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER;
      }else
      {
        void* bufferData = buffer.Map(0, sizeof(T_array_header), gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_RANGE);
        std::memcpy(bufferData, pointerToHeaderStart, sizeof(T_array_header));
        buffer.Unmap();
      }
    }

    lightComponents.dirty = false;
  }

  void* bufferData = buffer.Map(firstByteToWrite, numberOfBytesToWrite, gl::Buffer::MapType::WRITE, writeFlag);
  std::memcpy(bufferData, static_cast<const quint8*>(dataToWrite)+firstByteToWrite, static_cast<size_t>(numberOfBytesToWrite));
  buffer.Unmap();
}


template<class T_LightComponent, class T_array_header, int block_to_update>
void SimpleShaderStorageBuffer<T_LightComponent,T_array_header, block_to_update>::bindShaderStorageBuffer(int bindingIndex)
{
  buffer.BindShaderStorageBuffer(bindingIndex);
}


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SIMPLESHADERSTORAGEBUFFER_HINL
