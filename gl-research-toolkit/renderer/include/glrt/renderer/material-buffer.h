#ifndef GLRT_RENDERER_MATERIALBUFFER_H
#define GLRT_RENDERER_MATERIALBUFFER_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>

#include <glrt/scene/resources/material.h>
#include <glrt/renderer/declarations.h>
#include <glrt/renderer/static-mesh-buffer-manager.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/toolkit/array.h>

namespace glrt {
namespace renderer {

class MaterialBuffer final
{
public:
  typedef scene::resources::Material Material;
  typedef Material::TypeFlag TypeFlag;
  typedef Material::Type Type;

  const scene::resources::ResourceManager& resourceManager;
  const GLuint64 blockOffset;
  const GLuint64 totalCapacity;

  quint16 numAddresses = 0;
  gl::Buffer buffer;

  MaterialBuffer(scene::resources::ResourceManager& resourceManager, quint16 capacity);
  ~MaterialBuffer();

  MaterialBuffer(MaterialBuffer&&)=delete;
  MaterialBuffer&operator=(MaterialBuffer&&)=delete;

  MaterialBuffer(const MaterialBuffer&)=delete;
  MaterialBuffer&operator=(const MaterialBuffer&)=delete;

  void map(quint16 maxNumMaterials);
  Material::Type append(const Uuid<Material>& materialUuid);
  void unmap();

private:
  struct BlockSize
  {
    int blockOffset;
    int dataSize;

    template<typename T>
    static BlockSize forType();
    static BlockSize forType(Type type);
    static int expectedBlockOffset();
  };

  byte* mapped_data = nullptr;
};


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_MATERIALBUFFER_H
