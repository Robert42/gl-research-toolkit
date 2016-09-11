#include <glrt/renderer/deferred-renderer.h>
#include <glrt/renderer/debugging/debugging-posteffect.h>
#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace renderer {

DeferredRenderer::DeferredRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager, debugging::ShaderDebugPrinter* debugPrinter)
  : Renderer(videoResolution, scene, resourceManager->staticMeshBufferManager, debugPrinter),
    depth(videoResolution.x, videoResolution.y, gl::TextureFormat::DEPTH_COMPONENT32F),
    worldNormal_normalLength_Texture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA16F),
    baseColor_metalMask_Texture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA8),
    emission_reflectance_Texture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA8),
    occlusion_smoothness_Texture(videoResolution.x, videoResolution.y, gl::TextureFormat::RG8),
    mrt_framebuffer({gl::FramebufferObject::Attachment(&worldNormal_normalLength_Texture),
                    gl::FramebufferObject::Attachment(&baseColor_metalMask_Texture),
                    gl::FramebufferObject::Attachment(&emission_reflectance_Texture),
                    gl::FramebufferObject::Attachment(&occlusion_smoothness_Texture)},
                    gl::FramebufferObject::Attachment(&depth), false),
    glProgram_CopyFrameToBackBuffer("copy-deferred-framebuffer", QDir(GLRT_SHADER_DIR"/materials/implementation"))
{
  const Material::Type PLAIN_COLOR = Material::TypeFlag::PLAIN_COLOR | Material::TypeFlag::OPAQUE | Material::TypeFlag::VERTEX_SHADER_UNIFORM;
  const Material::Type SPHERE_AREA_LIGHT =  Material::TypeFlag::SPHERE_LIGHT;
  const Material::Type RECT_LIGHT_LIGHT = Material::TypeFlag::RECT_LIGHT;
  const Material::Type TEXTURED_OPAQUE = Material::TypeFlag::TEXTURED | Material::TypeFlag::OPAQUE | Material::TypeFlag::FRAGMENT_SHADER_UNIFORM;
  const Material::Type TEXTURED_MASKED_TWO_SIDED = Material::TypeFlag::TEXTURED | Material::TypeFlag::MASKED | Material::TypeFlag::TWO_SIDED | Material::TypeFlag::FRAGMENT_SHADER_UNIFORM;

  int plainColorShader = appendMaterialShader(preprocessorBlock(), {PLAIN_COLOR}, Pass::GBUFFER_FILL_PASS);
  int sphereLightShader = appendMaterialShader(preprocessorBlock(), {SPHERE_AREA_LIGHT}, Pass::GBUFFER_FILL_PASS);
  int rectLightShader = appendMaterialShader(preprocessorBlock(), {RECT_LIGHT_LIGHT}, Pass::GBUFFER_FILL_PASS);
  int texturedShader = appendMaterialShader(preprocessorBlock(), {TEXTURED_OPAQUE}, Pass::GBUFFER_FILL_PASS);
  int maskedTwoSidedShader = appendMaterialShader(preprocessorBlock(), {TEXTURED_MASKED_TWO_SIDED}, Pass::GBUFFER_FILL_PASS);

  MaterialState::Flags fillGbufferPassFlags = MaterialState::Flags::DEPTH_WRITE|MaterialState::Flags::COLOR_WRITE|MaterialState::Flags::ALPHA_WRITE|MaterialState::Flags::DEPTH_TEST;

  MaterialState::Flags maskedTwoSidedFlags = MaterialState::Flags::NO_FACE_CULLING;

  appendMaterialState(&mrt_framebuffer, {PLAIN_COLOR}, Pass::GBUFFER_FILL_PASS, plainColorShader, fillGbufferPassFlags);
  appendMaterialState(&mrt_framebuffer, {SPHERE_AREA_LIGHT}, Pass::GBUFFER_FILL_PASS, sphereLightShader, fillGbufferPassFlags);
  appendMaterialState(&mrt_framebuffer, {RECT_LIGHT_LIGHT}, Pass::GBUFFER_FILL_PASS, rectLightShader, fillGbufferPassFlags);
  appendMaterialState(&mrt_framebuffer, {TEXTURED_OPAQUE}, Pass::GBUFFER_FILL_PASS, texturedShader, fillGbufferPassFlags);
  appendMaterialState(&mrt_framebuffer, {TEXTURED_MASKED_TWO_SIDED}, Pass::GBUFFER_FILL_PASS, maskedTwoSidedShader, fillGbufferPassFlags | maskedTwoSidedFlags);

  GLuint64 textureHandle[5];
  textureHandle[0] = GL_RET_CALL(glGetTextureHandleNV, worldNormal_normalLength_Texture.GetInternHandle());
  textureHandle[1] = GL_RET_CALL(glGetTextureHandleNV, baseColor_metalMask_Texture.GetInternHandle());
  textureHandle[2] = GL_RET_CALL(glGetTextureHandleNV, emission_reflectance_Texture.GetInternHandle());
  textureHandle[3] = GL_RET_CALL(glGetTextureHandleNV, occlusion_smoothness_Texture.GetInternHandle());
  textureHandle[4] = GL_RET_CALL(glGetTextureHandleNV, depth.GetInternHandle());
  GL_CALL(glMakeTextureHandleResidentNV, textureHandle[0]);
  GL_CALL(glMakeTextureHandleResidentNV, textureHandle[1]);
  GL_CALL(glMakeTextureHandleResidentNV, textureHandle[2]);
  GL_CALL(glMakeTextureHandleResidentNV, textureHandle[3]);
  GL_CALL(glMakeTextureHandleResidentNV, textureHandle[4]);

  framebufferTextureHandlesBuffer = std::move(gl::Buffer(5*sizeof(GLuint64), gl::Buffer::IMMUTABLE, textureHandle));

  mrt_framebuffer.Bind(true);
  glrt::renderer::debugging::DebuggingPosteffect::init(&mrt_framebuffer, this);
  mrt_framebuffer.BindBackBuffer();
}

DeferredRenderer::~DeferredRenderer()
{
  glrt::renderer::debugging::DebuggingPosteffect::deinit();
}

void DeferredRenderer::prepareFramebuffer()
{
  mrt_framebuffer.Bind(true);

  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void DeferredRenderer::applyFramebuffer()
{
  mrt_framebuffer.BindBackBuffer();

  glProgram_CopyFrameToBackBuffer.glProgram.use();
  framebufferTextureHandlesBuffer.BindUniformBuffer(UNIFORM_BINDING_GBUFFER_BLOCK);
  GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
  gl::Program::useNone();
}

QSet<QString> DeferredRenderer::preprocessorBlock()
{
  return {"#define DEFERRED_RENDERER"};
}


} // namespace renderer
} // namespace glrt
