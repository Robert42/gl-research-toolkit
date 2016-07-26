#ifndef GLRT_RENDERER_MANAGEDGLBUFFER_INL
#define GLRT_RENDERER_MANAGEDGLBUFFER_INL

#include "managed-gl-buffer.h"

namespace glrt {
namespace renderer {

template<typename T_element, typename T_CapacityTraits, typename T_header_traits>
ManagedGLBuffer<T_element, T_CapacityTraits, T_header_traits>::ManagedGLBuffer()
  : gpuAddressChanged(true)
{
}

template<typename T_element, typename T_CapacityTraits, typename T_header_traits>
void ManagedGLBuffer<T_element, T_CapacityTraits, T_header_traits>::markElementDirty(int index)
{
  this->first_dirty_byte = glm::min(this->first_dirty_byte,
                                    byte_for_index(index));
}

template<typename T_element, typename T_CapacityTraits, typename T_header_traits>
void ManagedGLBuffer<T_element, T_CapacityTraits, T_header_traits>::setHeader(const header_type& header)
{
  header.set_header(&first_dirty_byte, &buffer, header);
}

template<typename T_element, typename T_CapacityTraits, typename T_header_traits>
void ManagedGLBuffer<T_element, T_CapacityTraits, T_header_traits>::setNumElements(int numElements)
{
  Q_ASSERT(numElements >= 0);

  header.set_n_elements(&first_dirty_byte, &buffer, numElements);

  int newCapacity = T_CapacityTraits::recalc_capacity(buffer.GetSize(),
                                                      T_header_traits::header_size() + numElements*sizeof(T_element));

  if(newCapacity != buffer.GetSize())
  {
    this->first_dirty_byte = 0;
    if(newCapacity > 0)
      this->buffer = std::move(gl::Buffer(newCapacity, gl::Buffer::MAP_WRITE));
    else
      this->buffer = std::move(gl::Buffer());
    gpuAddressChanged = true;
  }else
  {
    this->first_dirty_byte = glm::min<decltype(first_dirty_byte)>(this->first_dirty_byte, this->totalNumberBytes());
  }
}

template<typename T_element, typename T_CapacityTraits, typename T_header_traits>
int ManagedGLBuffer<T_element, T_CapacityTraits, T_header_traits>::numElements() const
{
  return header.n_elements();
}

template<typename T_element, typename T_CapacityTraits, typename T_header_traits>
int ManagedGLBuffer<T_element, T_CapacityTraits, T_header_traits>::byte_for_index(int index)
{
  return T_header_traits::header_size() + sizeof(T_element) * index;
}

template<typename T_element, typename T_CapacityTraits, typename T_header_traits>
int ManagedGLBuffer<T_element, T_CapacityTraits, T_header_traits>::firstElementToCopy() const
{
  return glm::max<int>(first_dirty_byte-T_header_traits::header_size(), 0) / sizeof(T_element);
}

template<typename T_element, typename T_CapacityTraits, typename T_header_traits>
bool ManagedGLBuffer<T_element, T_CapacityTraits, T_header_traits>::needsUpdate() const
{
  Q_ASSERT(first_dirty_byte <= this->totalNumberBytes());
  return this->totalNumberBytes() != first_dirty_byte;
}

/*!
\warning This method is dangerous. The returned buffer is only valid beginning
with the element which was marked dirty with the markElementDirty() command
*/
template<typename T_element, typename T_CapacityTraits, typename T_header_traits>
T_element* ManagedGLBuffer<T_element, T_CapacityTraits, T_header_traits>::Map()
{
  const GLsizeiptr total_number_bytes = this->totalNumberBytes();

  Q_ASSERT(first_dirty_byte <= total_number_bytes);
  Q_ASSERT(first_dirty_byte==0 || (first_dirty_byte-T_header_traits::header_size()) % sizeof(T_element) == 0);

  GLsizeiptr nDirtyBytes = total_number_bytes - first_dirty_byte;

  Q_ASSERT(nDirtyBytes>0); // Note: please check with needsUpdate, whether Map()Unmap() is necessary

  byte* whole_buffer = reinterpret_cast<byte*>(this->buffer.Map(first_dirty_byte, nDirtyBytes, gl::Buffer::MapType::WRITE, first_dirty_byte==0 ? gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER : gl::Buffer::MapWriteFlag::INVALIDATE_RANGE));

  header.copy_to(whole_buffer, first_dirty_byte);

  Q_ASSERT(reinterpret_cast<size_t>(whole_buffer) >= first_dirty_byte);

  return reinterpret_cast<T_element*>(whole_buffer+T_header_traits::header_size()-first_dirty_byte);
}

template<typename T_element, typename T_CapacityTraits, typename T_header_traits>
GLsizeiptr ManagedGLBuffer<T_element, T_CapacityTraits, T_header_traits>::totalNumberBytes() const
{
  return T_header_traits::header_size() + header.n_elements()*sizeof(T_element);
}

template<typename T_element, typename T_CapacityTraits, typename T_header_traits>
void ManagedGLBuffer<T_element, T_CapacityTraits, T_header_traits>::Unmap()
{
  this->buffer.Unmap();

  first_dirty_byte = this->totalNumberBytes();
}

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_MANAGEDGLBUFFER_INL
