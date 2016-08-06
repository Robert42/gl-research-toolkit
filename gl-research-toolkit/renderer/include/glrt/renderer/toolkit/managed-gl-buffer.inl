#ifndef GLRT_RENDERER_MANAGEDGLBUFFER_INL
#define GLRT_RENDERER_MANAGEDGLBUFFER_INL

#include "managed-gl-buffer.h"

namespace glrt {
namespace renderer {

template<typename T_element>
ManagedGLBuffer<T_element>::ManagedGLBuffer(quint32 capacity)
  : buffer(sizeof(T_element)*capacity, gl::Buffer::MAP_WRITE)
{
}

template<typename T_element>
T_element* ManagedGLBuffer<T_element>::Map(quint32 numElements)
{
  return reinterpret_cast<T_element*>(this->buffer.Map(0, numElements*sizeof(T_element), gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_RANGE));
}

template<typename T_element>
void ManagedGLBuffer<T_element>::Unmap()
{
  this->buffer.Unmap();
}

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_MANAGEDGLBUFFER_INL
