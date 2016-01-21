#ifndef GLRT_RENDERER_MATERIALBUFFER_H
#define GLRT_RENDERER_MATERIALBUFFER_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>

#include <glrt/scene/resources/material.h>

namespace glrt {
namespace renderer {

class MaterialBuffer final
{
public:
  typedef scene::resources::Material::Type Type;

  const Type type;

  MaterialBuffer(Type type);

  MaterialBuffer(const MaterialBuffer&)=delete;
  MaterialBuffer(MaterialBuffer&&)=delete;
  MaterialBuffer&operator=(const MaterialBuffer&)=delete;
  MaterialBuffer&operator=(MaterialBuffer&&)=delete;

  void bind(int i);

private:
  gl::Buffer buffer;
  int blockOffset;
  int dataSize;

  template<typename T>
  void initBlockOffset();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_MATERIALBUFFER_H
