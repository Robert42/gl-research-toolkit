#include <glrt/renderer/deferred-renderer.h>

namespace glrt {
namespace renderer {

DeferredRenderer::DeferredRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager, debugging::ShaderDebugPrinter* debugPrinter)
  : Renderer(videoResolution, scene, resourceManager->staticMeshBufferManager, debugPrinter),
    depth(videoResolution.x, videoResolution.y, gl::TextureFormat::DEPTH_COMPONENT24),
    worldNormal_normalLength_Texture(videoResolution.x, videoResolution.y, gl::TextureFormat::R16F),
    baseColor_metalMask_Texture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA8),
    emission_reflectance_Texture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA8),
    occlusion_smoothness_Texture(videoResolution.x, videoResolution.y, gl::TextureFormat::RG8),
    mrt_framebuffer({gl::FramebufferObject::Attachment(&worldNormal_normalLength_Texture),
                    gl::FramebufferObject::Attachment(&baseColor_metalMask_Texture),
                    gl::FramebufferObject::Attachment(&emission_reflectance_Texture),
                    gl::FramebufferObject::Attachment(&occlusion_smoothness_Texture)},
                    gl::FramebufferObject::Attachment(&depth), false)
{
}

DeferredRenderer::~DeferredRenderer()
{
}

void DeferredRenderer::prepareFramebuffer()
{
  mrt_framebuffer.Bind(true);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void DeferredRenderer::applyFramebuffer()
{
  mrt_framebuffer.BindBackBuffer();

  // TODO draw fullscreen quad
}

QSet<QString> DeferredRenderer::preprocessorBlock()
{
  return {"#define DEFERRED_RENDERER"};
}


} // namespace renderer
} // namespace glrt
