#ifndef GLRT_RENDERER_TRANSFORMATIONBUFFER_H
#define GLRT_RENDERER_TRANSFORMATIONBUFFER_H

#include <glrt/renderer/dependencies.h>
#include <glrt/scene/scene-data.h>
#include <glrt/toolkit/array.h>

#include <glhelper/buffer.hpp>

namespace glrt {
namespace renderer {

class TransformationBuffer final
{
public:
  TransformationBuffer(quint16 capacity);
  ~TransformationBuffer();

  TransformationBuffer(TransformationBuffer&&);
  TransformationBuffer&operator=(TransformationBuffer&&);

  TransformationBuffer(const TransformationBuffer&)=delete;
  TransformationBuffer&operator=(const TransformationBuffer&)=delete;

  void update(quint16 begin, quint16 end, const scene::Scene::Data::Transformations& transformations);
  GLuint64 gpuAddressForInstance(quint16 i) const;

private:
  struct UniformData
  {
    glm::mat4 transformation;
  };

  int alignment;
  gl::Buffer buffer;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_TRANSFORMATIONBUFFER_H
