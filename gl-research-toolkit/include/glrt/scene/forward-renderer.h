#ifndef GLRT_SCENE_FORWARDRENDERER_H
#define GLRT_SCENE_FORWARDRENDERER_H

#include "scene-renderer.h"

namespace glrt {
namespace scene {

class ForwardRenderer final : public Renderer
{
public:
  ForwardRenderer(Scene* scene);

  Pass plainColorMeshes;
  Pass texturedMeshes;
  Pass maskedMeshes;
  Pass transparentMeshes;

  void renderImplementation() override;

  static QSet<QString> preprocessorBlock();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_FORWARDRENDERER_H
