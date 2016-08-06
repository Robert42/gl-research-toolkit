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
  bool gpuAddressChanged : 1;

  ManagedGLBuffer(quint32 capacity);

  T_element* Map(quint32 numElements);
  void Unmap();
};

} // namespace renderer
} // namespace glrt

#include "managed-gl-buffer.inl"

#endif // GLRT_RENDERER_MANAGEDGLBUFFER_H
