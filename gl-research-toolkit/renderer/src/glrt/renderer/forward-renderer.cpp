#include <glrt/renderer/forward-renderer.h>

namespace glrt {
namespace renderer {

ForwardRenderer::ForwardRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager, debugging::ShaderDebugPrinter* debugPrinter)
  : Renderer(videoResolution, scene, resourceManager->staticMeshBufferManager, debugPrinter),
    colorFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA8),
    depthFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::DEPTH24_STENCIL8),
    framebuffer(gl::FramebufferObject::Attachment(&colorFramebufferTexture), gl::FramebufferObject::Attachment(&depthFramebufferTexture), true)
{
  int opaqueDepthPrepassShader = appendMaterialShader(preprocessorBlock(), {Material::Type::PLAIN_COLOR, Material::Type::TEXTURED_OPAQUE}, Pass::DEPTH_PREPASS);
  int maskedDepthPrepassShader = appendMaterialShader(preprocessorBlock(), {Material::Type::TEXTURED_MASKED}, Pass::DEPTH_PREPASS);

  int plainColorShader = appendMaterialShader(preprocessorBlock(), {Material::Type::PLAIN_COLOR}, Pass::FORWARD_PASS);
  int texturedShader = appendMaterialShader(preprocessorBlock(), {Material::Type::TEXTURED_OPAQUE,Material::Type::TEXTURED_MASKED,Material::Type::TEXTURED_TRANSPARENT}, Pass::FORWARD_PASS);

  MaterialState::Flags commonAccelerationFlags = MaterialState::Flags::DEPTH_TEST | MaterialState::Flags::FACE_CULLING;

  MaterialState::Flags depthPrepassFlags = MaterialState::Flags::DEPTH_WRITE|commonAccelerationFlags;
  MaterialState::Flags forwardPassFlags = MaterialState::Flags::COLOR_WRITE|commonAccelerationFlags;

  appendMaterialState(&framebuffer, {Material::Type::PLAIN_COLOR, Material::Type::TEXTURED_OPAQUE}, Pass::DEPTH_PREPASS, opaqueDepthPrepassShader, depthPrepassFlags);
  appendMaterialState(&framebuffer, {Material::Type::TEXTURED_MASKED}, Pass::DEPTH_PREPASS, maskedDepthPrepassShader, depthPrepassFlags);

  appendMaterialState(&framebuffer, {Material::Type::PLAIN_COLOR}, Pass::FORWARD_PASS, plainColorShader, forwardPassFlags);
  appendMaterialState(&framebuffer, {Material::Type::TEXTURED_OPAQUE}, Pass::FORWARD_PASS, texturedShader, forwardPassFlags);
  appendMaterialState(&framebuffer, {Material::Type::TEXTURED_MASKED}, Pass::FORWARD_PASS, texturedShader, forwardPassFlags);
  appendMaterialState(&framebuffer, {Material::Type::TEXTURED_TRANSPARENT}, Pass::FORWARD_PASS, texturedShader, forwardPassFlags);
}

void ForwardRenderer::prepareFramebuffer()
{
  framebuffer.Bind(true);

  Q_UNUSED(framebuffer);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void ForwardRenderer::applyFramebuffer()
{
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

