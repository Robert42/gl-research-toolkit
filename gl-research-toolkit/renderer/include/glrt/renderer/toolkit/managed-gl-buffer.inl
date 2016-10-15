#ifndef GLRT_RENDERER_MANAGEDGLBUFFER_INL
#define GLRT_RENDERER_MANAGEDGLBUFFER_INL

#include "managed-gl-buffer.h"

namespace glrt {
namespace renderer {

template<typename T_element>
ManagedGLBuffer<T_element>::ManagedGLBuffer(quint32 capacity)
  : buffer(sizeof(T_element)*capacity, gl::Buffer::MAP_WRITE)
{
  Q_ASSERT(capacity != 0);
}

template<typename T_element>
const T_element* ManagedGLBuffer<T_element>::MapReadOnly()
{
  return reinterpret_cast<T_element*>(this->buffer.Map(gl::Buffer::MapType::READ, gl::Buffer::MapWriteFlag::NONE));
}

template<typename T_element>
T_element* ManagedGLBuffer<T_element>::Map()
{
  return reinterpret_cast<T_element*>(this->buffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
}

template<typename T_element>
T_element* ManagedGLBuffer<T_element>::Map(quint32 numElements)
{
  if(Q_LIKELY(numElements != 0))
    return reinterpret_cast<T_element*>(this->buffer.Map(0, numElements*sizeof(T_element), gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_RANGE));
  else
    return nullptr;
}

template<typename T_element>
void ManagedGLBuffer<T_element>::Unmap()
{
  if(Q_LIKELY(this->buffer.isMapped()))
    this->buffer.Unmap();
}

template<typename T_element>
GLuint64 ManagedGLBuffer<T_element>::gpuBufferAddress() const
{
  return this->buffer.gpuBufferAddress();
}

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_MANAGEDGLBUFFER_INL
