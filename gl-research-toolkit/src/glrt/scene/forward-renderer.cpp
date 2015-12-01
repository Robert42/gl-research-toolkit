#include <glrt/scene/forward-renderer.h>

namespace glrt {
namespace scene {

ForwardRenderer::ForwardRenderer(Scene* scene)
  : Renderer(scene),
    plainColorMeshes(this, "plain-color-materials", preprocessorBlock()),
    texturedMeshes(this, "textured-meshes", preprocessorBlock()),
    maskedMeshes(this, "masked-meshes", preprocessorBlock()),
    transparentMeshes(this, "transparent-meshes", preprocessorBlock())
{
}


void ForwardRenderer::renderImplementation()
{
  plainColorMeshes.render();
  texturedMeshes.render();
  maskedMeshes.render();
  transparentMeshes.render();
}


QStringList ForwardRenderer::preprocessorBlock()
{
  return {"#define FORWARD_RENDERER"};
}


} // namespace scene
} // namespace glrt

