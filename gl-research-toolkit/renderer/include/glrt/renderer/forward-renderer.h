#ifndef GLRT_RENDERER_FORWARDRENDERER_H
#define GLRT_RENDERER_FORWARDRENDERER_H

#include "scene-renderer.h"

namespace glrt {
namespace renderer {

class ForwardRenderer final : public Renderer
{
public:
  ForwardRenderer(scene::Scene* scene);

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
