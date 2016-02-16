#include <glrt/renderer/forward-renderer.h>

namespace glrt {
namespace renderer {

ForwardRenderer::ForwardRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager)
  : Renderer(scene, resourceManager->staticMeshBufferManager),
    videoResolution(videoResolution),
    colorFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA8),
    depthFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::DEPTH24_STENCIL8),
    framebuffer(gl::FramebufferObject::Attachment(&colorFramebufferTexture), gl::FramebufferObject::Attachment(&depthFramebufferTexture), true),
    plainColorMeshes(this, scene::resources::Material::Type::PLAIN_COLOR, "plain-color-materials", preprocessorBlock()),
    texturedMeshes(this, scene::resources::Material::Type::TEXTURED_OPAQUE, "textured-meshes", preprocessorBlock()),
    maskedMeshes(this, scene::resources::Material::Type::TEXTURED_MASKED, "masked-meshes", preprocessorBlock()),
    transparentMeshes(this, scene::resources::Material::Type::TEXTURED_TRANSPARENT, "transparent-meshes", preprocessorBlock())
{
}


void ForwardRenderer::renderImplementation()
{
  framebuffer.Bind(true);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  plainColorMeshes.render();
  texturedMeshes.render();
  maskedMeshes.render();
  transparentMeshes.render();

  framebuffer.BindBackBuffer();

  framebuffer.BindRead();
  GL_CALL(glBlitFramebuffer, 0, 0, videoResolution.x, videoResolution.y, 0, 0, videoResolution.x, videoResolution.y, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT, GL_NEAREST);
  framebuffer.UnbindRead();
}


QSet<QString> ForwardRenderer::preprocessorBlock()
{
  return {"#define FORWARD_RENDERER"};
}


} // namespace renderer
} // namespace glrt

