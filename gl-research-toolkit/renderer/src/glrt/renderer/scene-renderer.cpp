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
    sceneVertexUniformBuffer(sizeof(SceneVertexUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    sceneFragmentUniformBuffer(sizeof(SceneFragmentUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr)
{
  fillCameraUniform(scene::CameraParameter());
  updateCameraUniform();
}

Renderer::~Renderer()
{
}

void Renderer::render()
{
#if GLRT_ENABLE_SCENE_RENDERING
  if(needRerecording())
    recordCommandlist();
  staticMeshRenderer.update();
#endif

  updateCameraUniform();
  clearFramebuffer();

  workaroundFramebuffer.Bind(false);
#if GLRT_ENABLE_SCENE_RENDERING
  commandList.call();
#endif
  callExtraCommandLists();
  workaroundFramebuffer.BindBackBuffer();

  applyFramebuffer();

  // #TODO: when usign command lists, is it possible to use the same biningsindex, if it's once vertex and once fragment shader?
  sceneVertexUniformBuffer.BindUniformBuffer(UNIFORM_BINDING_SCENE_VERTEX_BLOCK);
  sceneFragmentUniformBuffer.BindUniformBuffer(UNIFORM_BINDING_SCENE_FRAGMENT_BLOCK);
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
  materialShader->shader.shaderObject.Activate();
  // #TODO decouple the state capture and framebuffer from the shader: we should be able to use the same shader for different states and different statebuffers
  materialShader->stateCapture = std::move(gl::StatusCapture::capture(gl::StatusCapture::Mode::TRIANGLES));
  materialShader->framebuffer = framebuffer;
  framebuffer->BindBackBuffer();
  gl::ShaderObject::Deactivate();
  StaticMeshBuffer::disableVertexArrays();
}

bool Renderer::needRerecording() const
{
  return staticMeshRenderer.needRerecording();
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
  recorder.append_token_UniformAddress(UNIFORM_BINDING_SCENE_VERTEX_BLOCK, gl::ShaderObject::ShaderType::VERTEX, sceneVertexUniformBuffer.gpuBufferAddress());
  recorder.append_token_UniformAddress(UNIFORM_BINDING_SCENE_FRAGMENT_BLOCK, gl::ShaderObject::ShaderType::FRAGMENT, sceneFragmentUniformBuffer.gpuBufferAddress());
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
  SceneVertexUniformBlock& sceneVertexUniformData =  *reinterpret_cast<SceneVertexUniformBlock*>(sceneVertexUniformBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  sceneVertexUniformData.view_projection_matrix = cameraParameter.projectionMatrix() * cameraParameter.viewMatrix();
  sceneVertexUniformBuffer.Unmap();

  SceneFragmentUniformBlock& sceneFragmentUniformData =  *reinterpret_cast<SceneFragmentUniformBlock*>(sceneFragmentUniformBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  sceneFragmentUniformData.camera_position = cameraParameter.position;
  sceneFragmentUniformData.lightData = lightUniformBuffer.updateLightData();
  sceneFragmentUniformBuffer.Unmap();
}

GLuint64 Renderer::sceneVertexUniformAddress() const
{
  return sceneVertexUniformBuffer.gpuBufferAddress();
}

GLuint64 Renderer::sceneFragmentUniformAddress() const
{
  return sceneFragmentUniformBuffer.gpuBufferAddress();
}


} // namespace renderer
} // namespace glrt

