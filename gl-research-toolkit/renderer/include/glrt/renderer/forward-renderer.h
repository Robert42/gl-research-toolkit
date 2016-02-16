#ifndef GLRT_RENDERER_FORWARDRENDERER_H
#define GLRT_RENDERER_FORWARDRENDERER_H

#include "scene-renderer.h"
#include <glrt/renderer/sample-resource-manager.h>
#include <glhelper/framebufferobject.hpp>
#include <glhelper/texture2d.hpp>

namespace glrt {
namespace renderer {

class ForwardRenderer final : public Renderer
{
public:
  ForwardRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager);

  glm::ivec2 videoResolution;
  gl::Texture2D colorFramebufferTexture;
  gl::Texture2D depthFramebufferTexture;
  gl::FramebufferObject framebuffer;

  Pass plainColorMeshes;
  Pass texturedMeshes;
  Pass maskedMeshes;
  Pass transparentMeshes;

  void renderImplementation() override;

  static QSet<QString> preprocessorBlock();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_FORWARDRENDERER_H
