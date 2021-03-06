#ifndef GLRT_RENDERER_MANAGEDGLBUFFER_H
#define GLRT_RENDERER_MANAGEDGLBUFFER_H

#include <glhelper/buffer.hpp>
#include <glrt/toolkit/array.h>

namespace glrt {
namespace renderer {

template<typename T_element>
class ManagedGLBuffer final
{
public:
  gl::Buffer buffer;

  ManagedGLBuffer(quint32 capacity);

  const T_element* MapReadOnly();
  T_element* Map();
  T_element* Map(quint32 numElements);
  void Unmap();

  GLuint64 gpuBufferAddress() const;
};

} // namespace renderer
} // namespace glrt

#include "managed-gl-buffer.inl"

#endif // GLRT_RENDERER_MANAGEDGLBUFFER_H
