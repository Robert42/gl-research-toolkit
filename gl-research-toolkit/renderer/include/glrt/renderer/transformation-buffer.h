#ifndef GLRT_RENDERER_TRANSFORMATIONBUFFER_H
#define GLRT_RENDERER_TRANSFORMATIONBUFFER_H

#include <glrt/renderer/dependencies.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/toolkit/array.h>

#include <glhelper/buffer.hpp>

namespace glrt {
namespace renderer {

class TransformationBuffer final
{
public:
  TransformationBuffer();
  ~TransformationBuffer();

  TransformationBuffer(TransformationBuffer&&);
  TransformationBuffer&operator=(TransformationBuffer&&);

  TransformationBuffer(const TransformationBuffer&)=delete;
  TransformationBuffer&operator=(const TransformationBuffer&)=delete;

  void init(const glrt::scene::StaticMeshComponent** components, int length);
  void update(int begin, int end, const scene::StaticMeshComponent** components, int length);
  GLuint64 gpuAddressForInstance(int i) const;

private:
  struct UniformData
  {
    glm::mat4 transformation;
  };

  gl::Buffer buffer;
  int alignment;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_TRANSFORMATIONBUFFER_H
