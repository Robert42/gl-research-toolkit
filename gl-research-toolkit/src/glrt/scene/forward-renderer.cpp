#include <glrt/scene/forward-renderer.h>

namespace glrt {
namespace scene {

ForwardRenderer::ForwardRenderer(Scene* scene)
  : Renderer(scene),
    plainColorMeshes(this, MaterialInstance::Type::PLAIN_COLOR, "plain-color-materials", preprocessorBlock()),
    texturedMeshes(this, MaterialInstance::Type::TEXTURED_OPAQUE, "textured-meshes", preprocessorBlock()),
    maskedMeshes(this, MaterialInstance::Type::TEXTURED_MASKED, "masked-meshes", preprocessorBlock()),
    transparentMeshes(this, MaterialInstance::Type::TEXTURED_TRANSPARENT, "transparent-meshes", preprocessorBlock())
{
}


void ForwardRenderer::renderImplementation()
{
  plainColorMeshes.render();
  texturedMeshes.render();
  maskedMeshes.render();
  transparentMeshes.render();
}


QSet<QString> ForwardRenderer::preprocessorBlock()
{
  return {"#define FORWARD_RENDERER"};
}


} // namespace scene
} // namespace glrt

