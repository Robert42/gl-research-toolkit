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
  typedef Material::Type Type;

  struct Initializer;

  MaterialBuffer();
  ~MaterialBuffer();

  MaterialBuffer(MaterialBuffer&&);
  MaterialBuffer&operator=(MaterialBuffer&&);

  MaterialBuffer(const MaterialBuffer&)=delete;
  MaterialBuffer&operator=(const MaterialBuffer&)=delete;

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

  gl::Buffer buffer;
};

struct MaterialBuffer::Initializer
{
  gl::CommandListRecorder& recorder;
  const glrt::scene::resources::ResourceManager& resourceManager;

  Initializer(gl::CommandListRecorder& recorder, const glrt::scene::resources::ResourceManager& resourceManager);

  void begin(int expectedNumberMaterials);
  int append(const Uuid<Material>& materialUuid);
  MaterialBuffer end();

  QMap<Uuid<Material>, GLuint64> gpuAddresses; // #TODO use this or remove this :)

private:
  Array<byte> data;
  gl::Buffer buffer;
};


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_MATERIALBUFFER_H
