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


Renderer::Renderer(const glm::ivec2& videoResolution, scene::Scene* scene, StaticMeshBufferManager* staticMeshBufferManager, debugging::ShaderDebugPrinter* debugPrinter)
  : scene(*scene),
    staticMeshBufferManager(*staticMeshBufferManager),
    visualizeCameras(debugging::VisualizationRenderer::debugSceneCameras(scene)),
    visualizeSphereAreaLights(debugging::VisualizationRenderer::debugSphereAreaLights(scene)),
    visualizeRectAreaLights(debugging::VisualizationRenderer::debugRectAreaLights(scene)),
    videoResolution(videoResolution),
    lightUniformBuffer(this->scene),
    staticMeshRenderer(this->scene, staticMeshBufferManager),
    sceneVertexUniformBuffer(sizeof(SceneVertexUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    sceneFragmentUniformBuffer(sizeof(SceneFragmentUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    _needRecapturing(true),
    debugPrinter(*debugPrinter)
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
  staticMeshRenderer.update();

  updateCameraUniform();
  prepareFramebuffer();

  commandList.call();

  applyFramebuffer();

  sceneVertexUniformBuffer.BindUniformBuffer(UNIFORM_BINDING_SCENE_VERTEX_BLOCK);
  visualizeCameras.render();
  visualizeSphereAreaLights.render();
  visualizeRectAreaLights.render();
}


int Renderer::appendMaterialShader(QSet<QString> preprocessorBlock, const QSet<Material::Type>& materialTypes, const Pass pass)
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

  ReloadableShader shader("material",
                          QDir(GLRT_SHADER_DIR"/materials"),
                          preprocessorBlock);
  materialShaders.append_move(std::move(shader));

  return materialShaders.length()-1;
}

void Renderer::appendMaterialState(gl::FramebufferObject* framebuffer, const QSet<Material::Type>& materialTypes, const Pass pass, int shader, MaterialState::Flags flags)
{
  materialStates.append_move(std::move(MaterialState(shader, flags)));

  MaterialState* materialShader = &materialStates.last();

  for(Material::Type m : materialTypes)
    materialShaderMetadata[qMakePair(pass, m)] = materialShader;

  materialShader->framebuffer = framebuffer;
  _needRecapturing = true;
}

bool Renderer::needRecapturing() const
{
  return _needRecapturing;
}

bool Renderer::needRerecording() const
{
  return staticMeshRenderer.needRerecording() || _needRecapturing;
}

void Renderer::captureStates()
{
  for(MaterialState& materialState : materialStates)
  {
    gl::FramebufferObject& framebuffer = *materialState.framebuffer;
    ReloadableShader& shader = materialShaders[materialState.shader];

    StaticMeshBuffer::enableVertexArrays();
    framebuffer.Bind(false);
    shader.shaderObject.Activate();
    materialState.activateStateForFlags();
    materialState.stateCapture = std::move(gl::StatusCapture::capture(gl::StatusCapture::Mode::TRIANGLES));
    materialState.deactivateStateForFlags();
    framebuffer.BindBackBuffer();
    gl::ShaderObject::Deactivate();
    StaticMeshBuffer::disableVertexArrays();
  }
}

void Renderer::recordCommandlist()
{
  if(needRecapturing())
    captureStates();

  glm::ivec2 commonTokenList;
  gl::CommandListRecorder recorder;

  recorder.beginTokenList();
  debugPrinter.recordBinding(recorder);
  recorder.append_token_Viewport(glm::uvec2(0), glm::uvec2(videoResolution));
  recorder.append_token_UniformAddress(UNIFORM_BINDING_SCENE_VERTEX_BLOCK, gl::ShaderObject::ShaderType::VERTEX, sceneVertexUniformBuffer.gpuBufferAddress());
  recorder.append_token_UniformAddress(UNIFORM_BINDING_SCENE_FRAGMENT_BLOCK, gl::ShaderObject::ShaderType::FRAGMENT, sceneFragmentUniformBuffer.gpuBufferAddress());
  commonTokenList = recorder.endTokenList();

  TokenRanges meshDrawRanges = staticMeshRenderer.recordCommandList(recorder, commonTokenList);

  for(auto i=materialShaderMetadata.begin(); i!=materialShaderMetadata.end(); ++i)
  {
    Material::Type materialType = i.key().second;
    const MaterialState& materialShader = *i.value();

    glm::ivec2 range;

    if(meshDrawRanges.tokenRangeNotMovable.contains(materialType))
    {
      range = meshDrawRanges.tokenRangeNotMovable[materialType];
      recorder.append_drawcall(range, &materialShader.stateCapture, materialShader.framebuffer);
    }

    if(meshDrawRanges.tokenRangeMovables.contains(materialType))
    {
      range = meshDrawRanges.tokenRangeMovables[materialType];
      recorder.append_drawcall(range, &materialShader.stateCapture, materialShader.framebuffer);
    }
  }

  commandList = gl::CommandListRecorder::compile(std::move(recorder));

  _needRecapturing = false;
}

void Renderer::allShadersReloaded()
{
  _needRecapturing = true;
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

