#ifndef GLRT_RENDERER_MATERIALBUFFER_H
#define GLRT_RENDERER_MATERIALBUFFER_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>

#include <glrt/scene/resources/material.h>
#include <glrt/renderer/declarations.h>

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

  void bind(int i);
  void clear();

private:
  gl::Buffer* buffer;
  int blockOffset;
  int dataSize;


  template<typename T>
  void initBlockOffset();
};

struct MaterialBuffer::Initializer
{
  MaterialBuffer& buffer;
  int materialsAdded;
  QVector<quint8> data;

  Initializer(MaterialBuffer& buffer, int expectedNumberMaterials);
  ~Initializer();

  void append(const Material& material);
};


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_MATERIALBUFFER_H
