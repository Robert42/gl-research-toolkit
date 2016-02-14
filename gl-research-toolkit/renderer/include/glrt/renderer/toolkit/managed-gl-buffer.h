#ifndef GLRT_RENDERER_MANAGEDGLBUFFER_H
#define GLRT_RENDERER_MANAGEDGLBUFFER_H

#include <glhelper/buffer.hpp>
#include <glrt/toolkit/array.h>

namespace glrt {
namespace renderer {
namespace implementation {

struct SimpleShaderStorageBuffer_DefaultHeader
{
  int n_elements;
  padding<int, 3> _padding;
};

struct ManagedGLBuffer_NoHeader
{
  typedef quint8 byte;

  static int header_size(){return 0;}

  int n_elements() const
  {
    return _n_elements;
  }

  void set_header(int* dirty_byte, gl::Buffer* buffer, const ManagedGLBuffer_NoHeader& other)
  {
    set_n_elements(dirty_byte, buffer, other.n_elements());
  }

  void set_n_elements(int* dirty_byte, gl::Buffer* buffer, int n_elements)
  {
    Q_UNUSED(dirty_byte);
    Q_UNUSED(buffer);
    this->_n_elements = n_elements;
  }

  void copy_to(byte* whole_buffer, int first_dirty_byte) const
  {
    Q_UNUSED(whole_buffer);
    Q_UNUSED(first_dirty_byte);
  }

private:
  int _n_elements;
};

template<typename T_header=SimpleShaderStorageBuffer_DefaultHeader, int minDistanceForDifferentSynchronizations=256>
struct ManagedGLBuffer_Header_With_Num_Elements
{
  static_assert(sizeof(T_header) < minDistanceForDifferentSynchronizations, "minDistanceForDifferentSynchronizationsmust be larger than the header type");

  typedef quint8 byte;

  T_header header;

  static int header_size(){return sizeof(T_header);}

  int n_elements() const
  {
    return header.n_elements;
  }

  void set_header(int* dirty_byte, gl::Buffer* buffer, const ManagedGLBuffer_Header_With_Num_Elements& other)
  {
    this->header = other.header;
    header_changed(dirty_byte, buffer);
  }

  void set_n_elements(int* dirty_byte, gl::Buffer* buffer, int n_elements)
  {
    if(header.n_elements != n_elements)
    {
      header.n_elements = n_elements;
      header_changed(dirty_byte, buffer);
    }
  }

  void copy_to(byte* whole_buffer, int first_dirty_byte) const
  {
    if(first_dirty_byte == 0)
    {
      std::memcpy(whole_buffer, &header, sizeof(T_header));
    }
  }

private:
  void header_changed(int* dirty_byte, gl::Buffer* buffer)
  {
    if(*dirty_byte<minDistanceForDifferentSynchronizations)
    {
      *dirty_byte = 0;
    }else
    {
      void* target = buffer->Map(0, header_size(), gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_RANGE);
      const void* src = this;
      std::memcpy(target, src, header_size());
      buffer->Unmap();
    }
  }
};

} // namespace implementation

template<typename T_element, typename T_CapacityTraits = ArrayCapacityTraits_Capacity_Blocks<>, typename T_header_traits=implementation::ManagedGLBuffer_NoHeader>
class ManagedGLBuffer
{
public:
  typedef T_header_traits header_type;

  gl::Buffer buffer;

  ManagedGLBuffer();

  void markElementDirty(int index);
  void setHeader(const header_type& header);
  void setNumElements(int numElements);

  T_element* Map();
  void Unmap();

private:
  typedef quint8 byte;

  int first_dirty_byte = 0;
  T_header_traits header;

  static int byte_for_index(int index);
};

} // namespace renderer
} // namespace glrt

#include "managed-gl-buffer.inl"

#endif // GLRT_RENDERER_MANAGEDGLBUFFER_H
