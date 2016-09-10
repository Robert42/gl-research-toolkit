#ifndef GLRT_RENDERER_DEFERREDRENDERER_H
#define GLRT_RENDERER_DEFERREDRENDERER_H

#include "scene-renderer.h"
#include <glrt/renderer/sample-resource-manager.h>

namespace glrt {
namespace renderer {

class DeferredRenderer final : public Renderer
{
public:
  DeferredRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager, debugging::ShaderDebugPrinter* debugPrinter);
  ~DeferredRenderer();

  gl::Texture2D depth;
  gl::Texture2D worldNormal_normalLength_Texture;
  gl::Texture2D baseColor_metalMask_Texture;
  gl::Texture2D emission_reflectance_Texture;
  gl::Texture2D occlusion_smoothness_Texture;
  gl::FramebufferObject mrt_framebuffer;

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

#endif // GLRT_RENDERER_DEFERREDRENDERER_H
