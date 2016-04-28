#ifndef GLRT_RENDERER_DEBUGGING_DEBUGGINGPOSTEFFECT_H
#define GLRT_RENDERER_DEBUGGING_DEBUGGINGPOSTEFFECT_H

#include <glrt/renderer/debugging/debug-renderer.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>
#include <glrt/renderer/scene-renderer.h>

#include <glrt/renderer/gl/status-capture.h>
#include <glhelper/buffer.hpp>

namespace glrt {
namespace renderer {
namespace debugging {

class DebuggingPosteffect
{
public:
  class Renderer;

  DebuggingPosteffect() = delete;

  static void init(gl::FramebufferObject* framebuffer, glrt::renderer::Renderer* renderer);
  static void deinit();

  static DebugRenderer orangeSphere(const glm::vec3& origin=glm::vec3(0), float radius = 1.f, bool depthTest=true);
  static DebugRenderer voxelGridBoundingBox(bool depthTest=true);
  static DebugRenderer voxelGridCubicRaymarch(bool depthTest = true);

private:
  struct SharedRenderingData
  {
    gl::FramebufferObject& framebuffer;
    glrt::renderer::Renderer& renderer;
    gl::Buffer vertexBuffer;

    SharedRenderingData(gl::FramebufferObject* framebuffer, glrt::renderer::Renderer* renderer);
    ~SharedRenderingData();

    SharedRenderingData(const SharedRenderingData&) = delete;
    SharedRenderingData(SharedRenderingData&&) = delete;
    void operator=(const SharedRenderingData&) = delete;
    void operator=(SharedRenderingData&&) = delete;
  };

  static QSharedPointer<SharedRenderingData> renderingData;
};

} // namespace debugging
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEBUGGING_DEBUGGINGPOSTEFFECT_H
