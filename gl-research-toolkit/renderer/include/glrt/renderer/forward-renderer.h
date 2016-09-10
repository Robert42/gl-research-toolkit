#ifndef GLRT_RENDERER_FORWARDRENDERER_H
#define GLRT_RENDERER_FORWARDRENDERER_H

#include "scene-renderer.h"
#include <glrt/renderer/sample-resource-manager.h>

namespace glrt {
namespace renderer {

class ForwardRenderer final : public Renderer
{
public:
  ForwardRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager, debugging::ShaderDebugPrinter* debugPrinter);
  ~ForwardRenderer();

  gl::Texture2D colorFramebufferTexture;
  gl::Texture2D depthFramebufferTexture;
  gl::FramebufferObject framebuffer;

protected:
  void prepareFramebuffer() override;
  void applyFramebuffer() override;

private:
  gl::Program  glProgram_CopyFrameToBackBuffer;
  gl::Buffer framebufferTextureHandlesBuffer;

  static QSet<QString> preprocessorBlock();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_FORWARDRENDERER_H
