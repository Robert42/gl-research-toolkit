#include <glrt/renderer/forward-renderer.h>

namespace glrt {
namespace renderer {

#define GLRT_ENABLE_COMMANDLISTTEST 1

ForwardRenderer::ForwardRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager)
  : Renderer(videoResolution, scene, resourceManager->staticMeshBufferManager),
    colorFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA8),
    depthFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::DEPTH24_STENCIL8),
    framebuffer(gl::FramebufferObject::Attachment(&colorFramebufferTexture), gl::FramebufferObject::Attachment(&depthFramebufferTexture), true)
{
#if GLRT_ENABLE_SCENE_RENDERING
  appendMaterialShader(&framebuffer, preprocessorBlock(), {Material::Type::PLAIN_COLOR, Material::Type::TEXTURED_OPAQUE}, Pass::DEPTH_PREPASS);
  appendMaterialShader(&framebuffer, preprocessorBlock(), {Material::Type::TEXTURED_MASKED}, Pass::DEPTH_PREPASS);

  appendMaterialShader(&framebuffer, preprocessorBlock(), {Material::Type::PLAIN_COLOR}, Pass::FORWARD_PASS);
  appendMaterialShader(&framebuffer, preprocessorBlock(), {Material::Type::TEXTURED_OPAQUE}, Pass::FORWARD_PASS);
  appendMaterialShader(&framebuffer, preprocessorBlock(), {Material::Type::TEXTURED_MASKED}, Pass::FORWARD_PASS);
  appendMaterialShader(&framebuffer, preprocessorBlock(), {Material::Type::TEXTURED_TRANSPARENT}, Pass::FORWARD_PASS);
#endif

#if GLRT_ENABLE_COMMANDLISTTEST
  commandListTest = CommandListTest::AcceptGivenFramebuffer::Ptr(new CommandListTest::OrangeStaticMesh(&framebuffer, sceneVertexUniformAddress(), resourceManager->staticMeshBufferManager->meshForUuid(SampleResourceManager::suzanneLowPoly())));
  commandListTest->captureStateNow(gl::StatusCapture::Mode::TRIANGLES);
  commandListTest->recordCommands();
#endif
}

void ForwardRenderer::clearFramebuffer()
{
  framebuffer.Bind(true);

  Q_UNUSED(framebuffer);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  framebuffer.BindBackBuffer();
}

void ForwardRenderer::callExtraCommandLists()
{
#if GLRT_ENABLE_COMMANDLISTTEST
  commandListTest->draw();
#endif
}

void ForwardRenderer::applyFramebuffer()
{
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

