#include <glrt/glsl/layout-constants.h>

#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/camera-component.h>
#include <glrt/scene/collect-scene-data.h>

#include <glrt/system.h>
#include <glrt/renderer/scene-renderer.h>
#include <glrt/renderer/toolkit/aligned-vector.h>
#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/renderer/debugging/debugging-posteffect.h>

namespace glrt {
namespace renderer {

Renderer::Renderer(const glm::ivec2& videoResolution, scene::Scene* scene, StaticMeshBufferManager* staticMeshBufferManager, debugging::ShaderDebugPrinter* debugPrinter)
  : scene(*scene),
    staticMeshBufferManager(*staticMeshBufferManager),
    debugPrinter(*debugPrinter),
    visualizeCameras(debugging::VisualizationRenderer::debugSceneCameras(scene)),
    visualizeSphereAreaLights(debugging::VisualizationRenderer::debugSphereAreaLights(scene)),
    visualizeRectAreaLights(debugging::VisualizationRenderer::debugRectAreaLights(scene)),
    visualizeWorldGrid(debugging::VisualizationRenderer::showWorldGrid()),
    visualizeVoxelGrids(debugging::VisualizationRenderer::debugVoxelGrids(scene)),
    visualizeVoxelBoundingSpheres(debugging::VisualizationRenderer::debugVoxelBoundingSpheres(scene)),
    visualizePosteffect_OrangeTest(debugging::DebuggingPosteffect::orangeSphere()),
    visualizePosteffect_Voxel_HighlightUnconveiledNegativeDistances(debugging::DebuggingPosteffect::voxelGridHighlightUnconveiledNegativeDistances()),
    visualizePosteffect_Voxel_BoundingBox(debugging::DebuggingPosteffect::voxelGridBoundingBox()),
    visualizePosteffect_Voxel_Cubic_raymarch(debugging::DebuggingPosteffect::voxelGridCubicRaymarch()),
    visualizePosteffect_Distancefield_raymarch(debugging::DebuggingPosteffect::distanceFieldRaymarch()),
    visualizePosteffect_Distancefield_boundingSpheres_raymarch(debugging::DebuggingPosteffect::raymarchBoundingSpheresAsDistanceField()),
    videoResolution(videoResolution),
    lightUniformBuffer(this->scene),
    voxelUniformBuffer(this->scene),
    staticMeshRenderer(this->scene, staticMeshBufferManager),
    sceneUniformBuffer(sizeof(SceneUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    _needRecapturing(true),
    _adjustRoughness(false),
    _sdfShadows(false)
{
  fillCameraUniform(scene::CameraParameter());
  updateCameraUniform();

  debugDrawList_Backbuffer.connectTo(&visualizeCameras);
  debugDrawList_Backbuffer.connectTo(&visualizeSphereAreaLights);
  debugDrawList_Backbuffer.connectTo(&visualizeRectAreaLights);
  debugDrawList_Backbuffer.connectTo(&visualizeVoxelGrids);
  debugDrawList_Backbuffer.connectTo(&visualizeVoxelBoundingSpheres);
  debugDrawList_Backbuffer.connectTo(&visualizeWorldGrid);
  debugDrawList_Framebuffer.connectTo(&visualizePosteffect_OrangeTest);
  debugDrawList_Framebuffer.connectTo(&visualizePosteffect_Voxel_HighlightUnconveiledNegativeDistances);
  debugDrawList_Framebuffer.connectTo(&visualizePosteffect_Voxel_BoundingBox);
  debugDrawList_Framebuffer.connectTo(&visualizePosteffect_Voxel_Cubic_raymarch);
  debugDrawList_Framebuffer.connectTo(&visualizePosteffect_Distancefield_raymarch);
  debugDrawList_Framebuffer.connectTo(&visualizePosteffect_Distancefield_boundingSpheres_raymarch);

  setAdjustRoughness(true);
  setSDFShadows(true);
}

Renderer::~Renderer()
{
}

void Renderer::render()
{
  updateCameraUniform(); // This must be called before calling recordCommandlist (so the right numbe rof lights is known)

  if(Q_UNLIKELY(needRerecording()))
    recordCommandlist();
  staticMeshRenderer.update();

  prepareFramebuffer();

  commandList.call();

  debugDrawList_Framebuffer.render();

  applyFramebuffer();

  sceneUniformBuffer.BindUniformBuffer(UNIFORM_BINDING_SCENE_BLOCK);
  debugDrawList_Backbuffer.render();
}

void Renderer::update(float deltaTime)
{
  debugPosteffect.totalTime += deltaTime;
}

bool testFlagOnAll(const QSet<Material::Type>& types, Material::TypeFlag flag)
{
  int flagEnabled = 0;
  int flagDisabled = 0;
  for(Material::Type type : types)
    if(type.testFlag(flag))
      flagEnabled++;
    else
      flagDisabled++;

  return flagDisabled==0;
}

int Renderer::appendMaterialShader(QSet<QString> preprocessorBlock, const QSet<Material::Type>& materialTypes, const Pass pass)
{
  if(materialTypes.isEmpty())
  {
    Q_UNREACHABLE();
    return -1;
  }

  if(pass == Pass::DEPTH_PREPASS)
    preprocessorBlock.insert("#define DEPTH_PREPASS");
  else if(pass == Pass::FORWARD_PASS)
    preprocessorBlock.insert("#define FORWARD_PASS");
  else
    Q_UNREACHABLE();

  if(testFlagOnAll(materialTypes, Material::TypeFlag::MASKED))
  {
    preprocessorBlock.insert("#define OUTPUT_USES_ALPHA");
    preprocessorBlock.insert("#define MASKED");
  }else if(testFlagOnAll(materialTypes, Material::TypeFlag::TRANSPARENT))
  {
    preprocessorBlock.insert("#define OUTPUT_USES_ALPHA");
    preprocessorBlock.insert("#define TRANSPARENT");
  }else
  {
    preprocessorBlock.insert("#define OPAQUE");
  }

  if(testFlagOnAll(materialTypes, Material::TypeFlag::TWO_SIDED))
  {
    preprocessorBlock.insert("#define TWO_SIDED");
  }

  if(testFlagOnAll(materialTypes, Material::TypeFlag::AREA_LIGHT))
  {
    Q_ASSERT(!testFlagOnAll(materialTypes, Material::TypeFlag::TEXTURED));
    Q_ASSERT(!testFlagOnAll(materialTypes, Material::TypeFlag::PLAIN_COLOR));

    preprocessorBlock.insert("#define AREA_LIGHT");
    preprocessorBlock.insert("#define NO_LIGHTING"); // when rendering arealights themselves, don't calculate the lighting, because they are emissive only
    if(testFlagOnAll(materialTypes, Material::TypeFlag::SPHERE_LIGHT))
      preprocessorBlock.insert("#define SPHERE_LIGHT");
    if(testFlagOnAll(materialTypes, Material::TypeFlag::RECT_LIGHT))
      preprocessorBlock.insert("#define RECT_LIGHT");
  }

  if(testFlagOnAll(materialTypes, Material::TypeFlag::TEXTURED))
  {
    Q_ASSERT(!testFlagOnAll(materialTypes, Material::TypeFlag::PLAIN_COLOR));
    Q_ASSERT(!testFlagOnAll(materialTypes, Material::TypeFlag::AREA_LIGHT));

    preprocessorBlock.insert("#define TEXTURED");
  }
  if(testFlagOnAll(materialTypes, Material::TypeFlag::PLAIN_COLOR))
  {
    Q_ASSERT(!testFlagOnAll(materialTypes, Material::TypeFlag::TEXTURED));
    Q_ASSERT(!testFlagOnAll(materialTypes, Material::TypeFlag::AREA_LIGHT));

    preprocessorBlock.insert("#define PLAIN_COLOR");
  }

  preprocessorBlock.insert(QString("#define MASK_THRESHOLD %0").arg(maskAlphaThreshold));

  ReloadableShader shader("material",
                          QDir(GLRT_SHADER_DIR"/materials"),
                          preprocessorBlock);
  materialShaders.append_move(std::move(shader));

  return materialShaders.length()-1;
}

void Renderer::appendMaterialState(gl::FramebufferObject* framebuffer, const QSet<Material::Type>& materialTypes, const Pass pass, int shader, MaterialState::Flags flags)
{
  Q_ASSERT(framebuffer != nullptr);

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
  return staticMeshRenderer.needRerecording() || _needRecapturing || lightUniformBuffer.numVisibleChanged();
}

void Renderer::captureStates()
{
  for(MaterialState& materialState : materialStates)
  {
    gl::FramebufferObject& framebuffer = *materialState.framebuffer;
    ReloadableShader& shader = materialShaders[materialState.shader];

    Q_ASSERT(&framebuffer != nullptr);
    Q_ASSERT(&shader != nullptr);

    StaticMeshBuffer::enableVertexArrays();
    framebuffer.Bind(false);
    shader.glProgram.use();
    materialState.activateStateForFlags();
    materialState.stateCapture = std::move(gl::StatusCapture::capture(gl::StatusCapture::Mode::TRIANGLES));
    materialState.deactivateStateForFlags();
    framebuffer.BindBackBuffer();
    gl::Program::useNone();
    StaticMeshBuffer::disableVertexArrays();
  }
}

void Renderer::recordLightVisualization(gl::CommandListRecorder& recorder, Material::Type materialType, const MaterialState& materialShader, const glm::ivec2& commonTokenList)
{
  if(materialType.testFlag(Material::TypeFlag::AREA_LIGHT))
  {
    glm::ivec2 range;

    StaticMeshBuffer* staticMesh = nullptr;
    quint32 numLights;
    if(materialType.testFlag(Material::TypeFlag::SPHERE_LIGHT))
    {
      staticMesh = staticMeshBufferManager.meshForUuid(glrt::scene::resources::uuids::unitSphereMesh);
      numLights = lightUniformBuffer.numVisibleSphereAreaLights();
    }else if(materialType.testFlag(Material::TypeFlag::RECT_LIGHT))
    {
      staticMesh = staticMeshBufferManager.meshForUuid(glrt::scene::resources::uuids::unitRectMesh);
      numLights = lightUniformBuffer.numVisibleRectAreaLights();
    }else
      Q_UNREACHABLE();

    Q_ASSERT(numLights <= std::numeric_limits<int>::max());

    if(numLights > 0)
    {
      recorder.beginTokenListWithCopy(commonTokenList);
      staticMesh->recordBind(recorder);
      staticMesh->recordDrawInstances(recorder, 0, int(numLights));
      range = recorder.endTokenList();
      recorder.append_drawcall(range, &materialShader.stateCapture, materialShader.framebuffer);
    }
  }
}

void Renderer::recordCommandlist()
{
  if(Q_UNLIKELY(needRecapturing()))
    captureStates();

  lightUniformBuffer.updateNumberOfLights();

  glm::ivec2 commonTokenList;
  gl::CommandListRecorder recorder;

  recorder.beginTokenList();
  debugPrinter.recordBinding(recorder);
  recorder.append_token_Viewport(glm::uvec2(0), glm::uvec2(videoResolution));
  recorder.append_token_UniformAddress(UNIFORM_BINDING_SCENE_BLOCK, gl::ShaderType::VERTEX, sceneUniformBuffer.gpuBufferAddress());
  recorder.append_token_UniformAddress(UNIFORM_BINDING_SCENE_BLOCK, gl::ShaderType::FRAGMENT, sceneUniformBuffer.gpuBufferAddress());
  commonTokenList = recorder.endTokenList();

  TokenRanges meshDrawRanges = staticMeshRenderer.recordCommandList(recorder, commonTokenList);
  QSet<Material::Type> unusedMaterialTypes = meshDrawRanges.tokenRangeMovables.keys().toSet() | meshDrawRanges.tokenRangeNotMovable.keys().toSet();

  for(auto i=materialShaderMetadata.begin(); i!=materialShaderMetadata.end(); ++i)
  {
    Material::Type materialType = i.key().second;
    const MaterialState& materialShader = *i.value();

    unusedMaterialTypes.remove(materialType);

    glm::ivec2 range;

    recordLightVisualization(recorder, materialType, materialShader, commonTokenList);

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

  for(Material::Type type : unusedMaterialTypes)
    qWarning() << "MATERIAL-WARNING: There are materials with the type" << Material::typeToString(type) << "which are not supported by the scene renderer";

  commandList = gl::CommandListRecorder::compile(std::move(recorder));

  _needRecapturing = false;

  scene.sceneRerecordedCommands();
}

void Renderer::allShadersReloaded()
{
  _needRecapturing = true;
}

void Renderer::updateCameraUniform()
{
  if(Q_UNLIKELY(!this->cameraComponent))
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

  if(Q_LIKELY(this->cameraComponent))
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
  SceneUniformBlock& sceneUniformData =  *reinterpret_cast<SceneUniformBlock*>(sceneUniformBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  sceneUniformData.camera_position = cameraParameter.position;
  sceneUniformData.view_projection_matrix = cameraParameter.projectionMatrix() * cameraParameter.viewMatrix();
  sceneUniformData.lightData = lightUniformBuffer.updateLightData();
  sceneUniformData.voxelHeader = voxelUniformBuffer.updateVoxelHeader();
  sceneUniformData.totalTime = debugPosteffect.totalTime;
  sceneUniformData.costsHeatvisionBlackLevel = costsHeatvisionBlackLevel;
  sceneUniformData.costsHeatvisionWhiteLevel = costsHeatvisionWhiteLevel;
  sceneUniformBuffer.Unmap();
}

GLuint64 Renderer::sceneUniformAddress() const
{
  return sceneUniformBuffer.gpuBufferAddress();
}

bool Renderer::adjustRoughness() const
{
  return _adjustRoughness;
}

void Renderer::setAdjustRoughness(bool adjustRoughness)
{
  if(_adjustRoughness != adjustRoughness)
  {
    _adjustRoughness = adjustRoughness;
    ReloadableShader::defineMacro("ADJUST_ROUGHNESS", adjustRoughness);
  }
}

bool Renderer::sdfShadows() const
{
  return _sdfShadows;
}

void Renderer::setSDFShadows(bool sdfShadows)
{
  if(_sdfShadows != sdfShadows)
  {
    _sdfShadows = sdfShadows;
    ReloadableShader::defineMacro("CONETRACED_SHADOW", sdfShadows);
  }
}


} // namespace renderer
} // namespace glrt

