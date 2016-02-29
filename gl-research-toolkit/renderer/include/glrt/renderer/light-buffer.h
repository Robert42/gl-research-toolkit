#ifndef GLRT_RENDERER_LIGHTBUFFER_H
#define GLRT_RENDERER_LIGHTBUFFER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/renderer/simple-shader-storage-buffer.h>

namespace glrt {
namespace renderer {

class LightBuffer
{
public:
  LightBuffer(scene::Scene& scene);
  ~LightBuffer();

  bool needRerecording() const;
  void update();

  void recordBinding(gl::CommandListRecorder& recorder, GLushort sphereAreaLightBindingIndex, GLushort rectAreaLightBindingIndex);
  void recordBinding(gl::CommandListRecorder& recorder);

private:
  SimpleShaderStorageBuffer<scene::SphereAreaLightComponent> sphereAreaShaderStorageBuffer;
  SimpleShaderStorageBuffer<scene::RectAreaLightComponent> rectAreaShaderStorageBuffer;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_LIGHTBUFFER_H
