#include <glrt/glsl/layout-constants.h>

#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/camera-component.h>
#include <glrt/scene/collect-scene-data.h>

#include <glrt/system.h>
#include <glrt/renderer/scene-renderer.h>
#include <glrt/renderer/toolkit/aligned-vector.h>
#include <glrt/renderer/toolkit/shader-compiler.h>

namespace glrt {
namespace renderer {


Renderer::Renderer(const glm::ivec2& videoResolution, scene::Scene* scene, StaticMeshBufferManager* staticMeshBufferManager)
  : scene(*scene),
    staticMeshBufferManager(*staticMeshBufferManager),
    visualizeCameras(debugging::VisualizationRenderer::debugSceneCameras(scene)),
    visualizeSphereAreaLights(debugging::VisualizationRenderer::debugSphereAreaLights(scene)),
    visualizeRectAreaLights(debugging::VisualizationRenderer::debugRectAreaLights(scene)),
    videoResolution(videoResolution),
    lightUniformBuffer(this->scene),
    staticMeshRenderer(this->scene, staticMeshBufferManager),
    workaroundFramebufferTexture(4, 4, gl::TextureFormat::R8I),
    workaroundFramebuffer(gl::FramebufferObject::Attachment(&workaroundFramebufferTexture)),
    cameraUniformBuffer(sizeof(CameraUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr)
{
  fillCameraUniform(scene::CameraParameter());
  updateCameraUniform();
}

Renderer::~Renderer()
{
}

void Renderer::render()
{
  if(needRerecording())
    recordCommandlist();

  updateCameraUniform();
  lightUniformBuffer.update();
  staticMeshRenderer.update();

  clearFramebuffer();

  workaroundFramebuffer.Bind(false);
  /* #TODO uncomment
  commandList.call();
  */
  callExtraCommandLists();
  workaroundFramebuffer.BindBackBuffer();

  applyFramebuffer();

  cameraUniformBuffer.BindUniformBuffer(UNIFORM_BINDING_SCENE_BLOCK);
  visualizeCameras.render();
  visualizeSphereAreaLights.render();
  visualizeRectAreaLights.render();
}

void Renderer::appendMaterialShader(gl::FramebufferObject* framebuffer, QSet<QString> preprocessorBlock, const QSet<Material::Type>& materialTypes, const Pass pass)
{
  if(pass == Pass::DEPTH_PREPASS)
    preprocessorBlock.insert("#define DEPTH_PREPASS");
  else if(pass == Pass::FORWARD_PASS)
    preprocessorBlock.insert("#define FORWARD_PASS");
  else
    Q_UNREACHABLE();

  if(materialTypes.contains(Material::Type::PLAIN_COLOR) || materialTypes.contains(Material::Type::TEXTURED_OPAQUE))
    preprocessorBlock.insert("#define OPAQUE");
  else
  {
    preprocessorBlock.insert("#define OUTPUT_USES_ALPHA");

    if(materialTypes.contains(Material::Type::TEXTURED_MASKED))
      preprocessorBlock.insert("#define MASKED");
    else if(materialTypes.contains(Material::Type::TEXTURED_TRANSPARENT))
      preprocessorBlock.insert("#define TRANSPARENT");
    else
      Q_UNREACHABLE();
  }

  if(materialTypes.contains(Material::Type::PLAIN_COLOR))
    preprocessorBlock.insert("#define PLAIN_COLOR");
  else if(materialTypes.contains(Material::Type::TEXTURED_OPAQUE)||materialTypes.contains(Material::Type::TEXTURED_MASKED)||materialTypes.contains(Material::Type::TEXTURED_TRANSPARENT))
    preprocessorBlock.insert("#define TEXTURED");
  else
    Q_UNREACHABLE();

  materialShaders.append_move(std::move(MaterialShader(preprocessorBlock)));

  MaterialShader* materialShader = &materialShaders.last();

  for(Material::Type m : materialTypes)
    materialShaderMetadata[qMakePair(pass, m)] = materialShader;

  StaticMeshBuffer::enableVertexArrays();
  framebuffer->Bind(false);
  // #TODO decouple the state capture and framebuffer from the shader: we should be able to use the same shader for different states and different statebuffers
  materialShader->stateCapture = std::move(gl::StatusCapture::capture(gl::StatusCapture::Mode::TRIANGLES));
  materialShader->framebuffer = framebuffer;
  framebuffer->BindBackBuffer();
  StaticMeshBuffer::disableVertexArrays();
}

bool Renderer::needRerecording() const
{
  return lightUniformBuffer.needRerecording() || staticMeshRenderer.needRerecording();
}

void Renderer::recordCommandlist()
{
  const glm::ivec2 videoResolution = glrt::System::windowSize();
  // #FIXME support different materials
  MaterialShader* materialShader = materialShaderMetadata[qMakePair(Pass::FORWARD_PASS, Material::Type::PLAIN_COLOR)];

  glm::ivec2 tokenRange;
  gl::CommandListRecorder recorder;

  recorder.beginTokenList();
  recorder.append_token_Viewport(glm::uvec2(0), glm::uvec2(videoResolution));
  recorder.append_token_UniformAddress(UNIFORM_BINDING_SCENE_BLOCK, gl::ShaderObject::ShaderType::VERTEX, cameraUniformBuffer.gpuBufferAddress());
  lightUniformBuffer.recordBinding(recorder);
  staticMeshRenderer.recordCommandList(recorder);
  tokenRange = recorder.endTokenList();

  recorder.append_drawcall(tokenRange, &materialShader->stateCapture, materialShader->framebuffer);

  commandList = gl::CommandListRecorder::compile(std::move(recorder));
}

void Renderer::updateCameraUniform()
{
  if(!this->cameraComponent)
  {
    Array<scene::CameraComponent*> cameraComponents = scene::collectAllComponentsWithType<scene::CameraComponent>(&scene);

    for(scene::CameraComponent* cameraComponent : cameraComponents)
    {
      if(cameraComponent->uuid == scene::uuids::debugCameraComponent)
      {
        this->cameraComponent = cameraComponent;
        break;
      }
    }
    if(!cameraComponent && !cameraComponents.isEmpty())
      this->cameraComponent = cameraComponents.first();
  }

  if(this->cameraComponent)
    updateCameraComponent(this->cameraComponent);
}

void Renderer::updateCameraComponent(scene::CameraComponent* cameraComponent)
{
  Q_ASSERT(cameraComponent != nullptr);
  cameraComponent->cameraParameter.aspect = float(videoResolution.x) / float(videoResolution.y);
  fillCameraUniform(cameraComponent->globalCameraParameter());
}

void Renderer::fillCameraUniform(const scene::CameraParameter& cameraParameter)
{
  CameraUniformBlock& cameraUniformData =  *reinterpret_cast<CameraUniformBlock*>(cameraUniformBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  cameraUniformData.view_projection_matrix = cameraParameter.projectionMatrix() * cameraParameter.viewMatrix();
  cameraUniformData.camera_position = cameraParameter.position;
  cameraUniformBuffer.Unmap();
}

GLuint64 Renderer::cameraUniformAddress() const
{
  return cameraUniformBuffer.gpuBufferAddress();
}


} // namespace renderer
} // namespace glrt

