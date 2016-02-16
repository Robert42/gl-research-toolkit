#include <glrt/renderer/forward-renderer.h>

namespace glrt {
namespace renderer {

ForwardRenderer::ForwardRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager)
  : Renderer(scene, resourceManager->staticMeshBufferManager),
    colorFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA8),
    depthFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::DEPTH32F_STENCIL8),
    framebuffer(gl::FramebufferObject::Attachment(&colorFramebufferTexture), gl::FramebufferObject::Attachment(&depthFramebufferTexture), true),
    plainColorMeshes(this, scene::resources::Material::Type::PLAIN_COLOR, "plain-color-materials", preprocessorBlock()),
    texturedMeshes(this, scene::resources::Material::Type::TEXTURED_OPAQUE, "textured-meshes", preprocessorBlock()),
    maskedMeshes(this, scene::resources::Material::Type::TEXTURED_MASKED, "masked-meshes", preprocessorBlock()),
    transparentMeshes(this, scene::resources::Material::Type::TEXTURED_TRANSPARENT, "transparent-meshes", preprocessorBlock())
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


} // namespace renderer
} // namespace glrt

