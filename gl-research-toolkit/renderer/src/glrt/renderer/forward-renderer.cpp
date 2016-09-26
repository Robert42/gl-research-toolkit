#include <glrt/renderer/forward-renderer.h>
#include <glrt/renderer/debugging/debugging-posteffect.h>
#include <glrt/renderer/toolkit/shader-compiler.h>

namespace glrt {
namespace renderer {


ForwardRenderer::ForwardRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager, debugging::ShaderDebugPrinter* debugPrinter)
  : Renderer(videoResolution, scene, resourceManager->staticMeshBufferManager, debugPrinter, {"#define FORWARD_RENDERER"}),
    colorFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA8),
    depthFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::DEPTH_COMPONENT32F),
    framebuffer(gl::FramebufferObject::Attachment(&colorFramebufferTexture), gl::FramebufferObject::Attachment(&depthFramebufferTexture), false),
    glProgram_CopyFrameToBackBuffer(std::move(ShaderCompiler::singleton().compileProgramFromFiles("copy-forward-framebuffer", QDir(GLRT_SHADER_DIR"/materials/implementation"))))
{
  const Material::Type PLAIN_COLOR = Material::TypeFlag::PLAIN_COLOR | Material::TypeFlag::OPAQUE | Material::TypeFlag::VERTEX_SHADER_UNIFORM;
  const Material::Type SPHERE_AREA_LIGHT =  Material::TypeFlag::SPHERE_LIGHT;
  const Material::Type RECT_LIGHT_LIGHT = Material::TypeFlag::RECT_LIGHT;
  const Material::Type TEXTURED_OPAQUE = Material::TypeFlag::TEXTURED | Material::TypeFlag::OPAQUE | Material::TypeFlag::FRAGMENT_SHADER_UNIFORM;
  const Material::Type TEXTURED_MASKED_TWO_SIDED = Material::TypeFlag::TEXTURED | Material::TypeFlag::MASKED | Material::TypeFlag::TWO_SIDED | Material::TypeFlag::FRAGMENT_SHADER_UNIFORM;
  const Material::Type TEXTURED_TRANSPARENT_TWO_SIDED = Material::TypeFlag::TEXTURED | Material::TypeFlag::TRANSPARENT | Material::TypeFlag::TWO_SIDED | Material::TypeFlag::FRAGMENT_SHADER_UNIFORM;

  int opaqueDepthPrepassShader = appendMaterialShader(preprocessorBlock(), {PLAIN_COLOR, TEXTURED_OPAQUE}, Pass::DEPTH_PREPASS);
  int sphereLightDepthPrepassShader = appendMaterialShader(preprocessorBlock(), {SPHERE_AREA_LIGHT}, Pass::DEPTH_PREPASS);
  int rectLightDepthPrepassShader = appendMaterialShader(preprocessorBlock(), {RECT_LIGHT_LIGHT}, Pass::DEPTH_PREPASS);
  int maskedDepthPrepassShader = appendMaterialShader(preprocessorBlock(), {TEXTURED_MASKED_TWO_SIDED}, Pass::DEPTH_PREPASS);

  int plainColorShader = appendMaterialShader(preprocessorBlock(), {PLAIN_COLOR}, Pass::FORWARD_PASS);
  int sphereLightShader = appendMaterialShader(preprocessorBlock(), {SPHERE_AREA_LIGHT}, Pass::FORWARD_PASS);
  int rectLightShader = appendMaterialShader(preprocessorBlock(), {RECT_LIGHT_LIGHT}, Pass::FORWARD_PASS);
  int texturedShader = appendMaterialShader(preprocessorBlock(), {TEXTURED_OPAQUE}, Pass::FORWARD_PASS);
  int maskedTwoSidedShader = appendMaterialShader(preprocessorBlock(), {TEXTURED_MASKED_TWO_SIDED}, Pass::FORWARD_PASS);
  int transparentTwoSidedShader = appendMaterialShader(preprocessorBlock(), {TEXTURED_TRANSPARENT_TWO_SIDED}, Pass::FORWARD_PASS);

  MaterialState::Flags commonAccelerationFlags = MaterialState::Flags::DEPTH_TEST;

  MaterialState::Flags depthPrepassFlags = MaterialState::Flags::DEPTH_WRITE|commonAccelerationFlags;
  MaterialState::Flags forwardPassFlags = MaterialState::Flags::COLOR_WRITE|commonAccelerationFlags;

  MaterialState::Flags maskedTwoSidedFlags = MaterialState::Flags::NO_FACE_CULLING | MaterialState::Flags::ALPHA_BLENDING;
  MaterialState::Flags transparentTwoSidedFlags = MaterialState::Flags::NO_FACE_CULLING | MaterialState::Flags::ALPHA_BLENDING;

  appendMaterialState(&framebuffer, {PLAIN_COLOR, TEXTURED_OPAQUE}, Pass::DEPTH_PREPASS, opaqueDepthPrepassShader, depthPrepassFlags);
  appendMaterialState(&framebuffer, {SPHERE_AREA_LIGHT}, Pass::DEPTH_PREPASS, sphereLightDepthPrepassShader, depthPrepassFlags);
  appendMaterialState(&framebuffer, {RECT_LIGHT_LIGHT}, Pass::DEPTH_PREPASS, rectLightDepthPrepassShader, depthPrepassFlags);
  appendMaterialState(&framebuffer, {TEXTURED_MASKED_TWO_SIDED}, Pass::DEPTH_PREPASS, maskedDepthPrepassShader, depthPrepassFlags | maskedTwoSidedFlags);

  appendMaterialState(&framebuffer, {PLAIN_COLOR}, Pass::FORWARD_PASS, plainColorShader, forwardPassFlags);
  appendMaterialState(&framebuffer, {SPHERE_AREA_LIGHT}, Pass::FORWARD_PASS, sphereLightShader, forwardPassFlags);
  appendMaterialState(&framebuffer, {RECT_LIGHT_LIGHT}, Pass::FORWARD_PASS, rectLightShader, forwardPassFlags);
  appendMaterialState(&framebuffer, {TEXTURED_OPAQUE}, Pass::FORWARD_PASS, texturedShader, forwardPassFlags);
  appendMaterialState(&framebuffer, {TEXTURED_MASKED_TWO_SIDED}, Pass::FORWARD_PASS, maskedTwoSidedShader, forwardPassFlags | maskedTwoSidedFlags);
  appendMaterialState(&framebuffer, {TEXTURED_TRANSPARENT_TWO_SIDED}, Pass::FORWARD_PASS, transparentTwoSidedShader, forwardPassFlags | transparentTwoSidedFlags);

  GLuint64 colorTextureHandle = GL_RET_CALL(glGetTextureHandleNV, colorFramebufferTexture.GetInternHandle());
  GL_CALL(glMakeTextureHandleResidentNV, colorTextureHandle);

  framebufferTextureHandlesBuffer = std::move(gl::Buffer(sizeof(GLuint64), gl::Buffer::IMMUTABLE, &colorTextureHandle));

  framebuffer.Bind(true);
  glrt::renderer::debugging::DebuggingPosteffect::init(&framebuffer, this);
  framebuffer.BindBackBuffer();
}

ForwardRenderer::~ForwardRenderer()
{
  glrt::renderer::debugging::DebuggingPosteffect::deinit();
}

void ForwardRenderer::prepareFramebuffer()
{
  framebuffer.Bind(true);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void ForwardRenderer::applyFramebuffer()
{
  framebuffer.BindBackBuffer();

  glProgram_CopyFrameToBackBuffer.use();
  framebufferTextureHandlesBuffer.BindUniformBuffer(0);
  GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
  gl::Program::useNone();
}

QSet<QString> ForwardRenderer::preprocessorBlock()
{
  return {"#define FORWARD_RENDERER"};
}


} // namespace renderer
} // namespace glrt

