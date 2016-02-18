#ifndef GLRT_RENDERER_MATERIALBUFFER_H
#define GLRT_RENDERER_MATERIALBUFFER_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>

#include <glrt/scene/resources/material.h>
#include <glrt/renderer/declarations.h>
#include <glrt/toolkit/array.h>

namespace glrt {
namespace renderer {

class MaterialBuffer final
{
public:
  typedef scene::resources::Material::Type Type;

  struct Initializer;

  const Type type;

  MaterialBuffer(Type type);
  ~MaterialBuffer();

  MaterialBuffer(const MaterialBuffer&)=delete;
  MaterialBuffer(MaterialBuffer&&)=delete;
  MaterialBuffer&operator=(const MaterialBuffer&)=delete;
  MaterialBuffer&operator=(MaterialBuffer&&)=delete;

  void clear();

private:
  struct BlockSize
  {
    int blockOffset;
    int dataSize;

    template<typename T>
    static BlockSize forType();
    static BlockSize forType(Type type);
  };

  gl::Buffer buffer;
  BlockSize blockSize;
};

struct MaterialBuffer::Initializer
{
  void begin(int expectedNumberMaterials, Type type);
  int append(const Material& material);
  MaterialBuffer&& end();

private:
  Array<byte> data;
  Type type = Type::PLAIN_COLOR;
  BlockSize blockSize;
};


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_MATERIALBUFFER_H
