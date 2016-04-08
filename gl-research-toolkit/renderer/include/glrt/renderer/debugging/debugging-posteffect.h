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

  static DebugRenderer orangeScreen(float zValue, float radius, bool depthTest); // #TODO
  static DebugRenderer overlayLightSources(bool depthTest); // #TODO implement posteffect for drawing light sources (don't forget gamma)
  // #TODO: Think about writing a debug (mesh based) renderer for lgith sources

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
