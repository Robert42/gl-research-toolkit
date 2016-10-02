#include <glrt/glsl/layout-constants.h>
#include <glrt/toolkit/profiler.h>

#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/camera-component.h>
#include <glrt/scene/collect-scene-data.h>

#include <glrt/system.h>
#include <glrt/renderer/scene-renderer.h>
#include <glrt/renderer/toolkit/aligned-vector.h>
#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/renderer/debugging/debugging-posteffect.h>

#define MAKE_IMAGE_ONLY_RESIDENT_IF_NECESSARY 1
#define MAKE_TEXTURE_NON_RESIDENT_BEFORE_COMPUTING 2

#define RESIDENCY_TACTIC 0
//#define RESIDENCY_TACTIC (MAKE_IMAGE_ONLY_RESIDENT_IF_NECESSARY | MAKE_TEXTURE_NON_RESIDENT_BEFORE_COMPUTING)

namespace glrt {
namespace renderer {

Renderer::Renderer(const glm::ivec2& videoResolution, scene::Scene* scene, StaticMeshBufferManager* staticMeshBufferManager, debugging::ShaderDebugPrinter* debugPrinter, const QSet<QString>& debug_posteffect_preprocessor)
  : scene(*scene),
    staticMeshBufferManager(*staticMeshBufferManager),
    debugPrinter(*debugPrinter),
    visualizeCameras(debugging::VisualizationRenderer::debugSceneCameras(scene)),
    visualizeSphereAreaLights(debugging::VisualizationRenderer::debugSphereAreaLights(scene)),
    visualizeRectAreaLights(debugging::VisualizationRenderer::debugRectAreaLights(scene)),
    visualizeVoxelGrids(debugging::VisualizationRenderer::debugVoxelGrids(scene)),
    visualizeVoxelBoundingSpheres(debugging::VisualizationRenderer::debugVoxelBoundingSpheres(scene)),
    visualizeSdfCandidateGrid(debugging::VisualizationRenderer::showSceneSdfCandidateGrid(scene)),
    visualizeSdfCandidateCell(debugging::VisualizationRenderer::showSceneSdfCandidatesForCell(scene)),
    visualizeBVH(debugging::VisualizationRenderer::showSceneBVH(scene)),
    visualizeBVH_Grid(debugging::VisualizationRenderer::showSceneBVH_Grid(scene)),
    visualizeWorldGrid(debugging::VisualizationRenderer::showWorldGrid()),
    visualizeUniformTest(debugging::VisualizationRenderer::showUniformTest()),
    visualizeBoundingBoxes(debugging::VisualizationRenderer::showMeshAABBs(scene)),
    visualizeSceneBoundingBox(debugging::VisualizationRenderer::showSceneAABB(scene)),
    visualizePosteffect_OrangeTest(debugging::DebuggingPosteffect::orangeSphere(debug_posteffect_preprocessor)),
    visualizePosteffect_Voxel_HighlightUnconveiledNegativeDistances(debugging::DebuggingPosteffect::voxelGridHighlightUnconveiledNegativeDistances(debug_posteffect_preprocessor)),
    visualizePosteffect_Voxel_BoundingBox(debugging::DebuggingPosteffect::voxelGridBoundingBox(debug_posteffect_preprocessor)),
    visualizePosteffect_Voxel_Cubic_raymarch(debugging::DebuggingPosteffect::voxelGridCubicRaymarch(debug_posteffect_preprocessor)),
    visualizePosteffect_Distancefield_raymarch(debugging::DebuggingPosteffect::distanceFieldRaymarch(debug_posteffect_preprocessor)),
    visualizePosteffect_Fallback_Distancefield_raymarch(debugging::DebuggingPosteffect::fallbackDistanceFieldRaymarch(debug_posteffect_preprocessor)),
    visualizePosteffect_Distancefield_boundingSpheres_raymarch(debugging::DebuggingPosteffect::raymarchBoundingSpheresAsDistanceField(debug_posteffect_preprocessor)),
    videoResolution(videoResolution),
    collectAmbientOcclusionToGrid1(GLRT_SHADER_DIR "/compute/collect-ambient-occlusion.cs", glm::ivec3(16, 16, 16*1), QSet<QString>({"#define COMPUTE_GRIDS"})),
    collectAmbientOcclusionToGrid2(GLRT_SHADER_DIR "/compute/collect-ambient-occlusion.cs", glm::ivec3(16, 16, 16*2), QSet<QString>({"#define COMPUTE_GRIDS"})),
    collectAmbientOcclusionToGrid3(GLRT_SHADER_DIR "/compute/collect-ambient-occlusion.cs", glm::ivec3(16, 16, 16*3), QSet<QString>({"#define COMPUTE_GRIDS"})),
    _update_grid_camera(true),
    _needRecapturing(true),
    sceneUniformBuffer(sizeof(SceneUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    aoCollectHeaderUniformBuffer(sizeof(AoCollectHeader), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    lightUniformBuffer(this->scene),
    voxelUniformBuffer(this->scene),
    staticMeshRenderer(this->scene, staticMeshBufferManager),
    _adjustRoughness(false),
    _sdfShadows(false)
{
  fillCameraUniform(scene::CameraParameter());
  updateCameraUniform();

  debugDrawList_Lines.connectTo(&visualizeCameras);
  debugDrawList_Lines.connectTo(&visualizeSphereAreaLights);
  debugDrawList_Lines.connectTo(&visualizeRectAreaLights);
  debugDrawList_Lines.connectTo(&visualizeVoxelGrids);
  debugDrawList_Lines.connectTo(&visualizeVoxelBoundingSpheres);
  debugDrawList_Lines.connectTo(&visualizeSdfCandidateGrid);
  debugDrawList_Lines.connectTo(&visualizeSdfCandidateCell);
  debugDrawList_Lines.connectTo(&visualizeBVH);
  debugDrawList_Lines.connectTo(&visualizeBVH_Grid);
  debugDrawList_Lines.connectTo(&visualizeWorldGrid);
  debugDrawList_Lines.connectTo(&visualizeUniformTest);
  debugDrawList_Lines.connectTo(&visualizeBoundingBoxes);
  debugDrawList_Lines.connectTo(&visualizeSceneBoundingBox);
  debugDrawList_Posteffects.connectTo(&visualizePosteffect_OrangeTest);
  debugDrawList_Posteffects.connectTo(&visualizePosteffect_Voxel_HighlightUnconveiledNegativeDistances);
  debugDrawList_Posteffects.connectTo(&visualizePosteffect_Voxel_BoundingBox);
  debugDrawList_Posteffects.connectTo(&visualizePosteffect_Voxel_Cubic_raymarch);
  debugDrawList_Posteffects.connectTo(&visualizePosteffect_Distancefield_raymarch);
  debugDrawList_Posteffects.connectTo(&visualizePosteffect_Fallback_Distancefield_raymarch);
  debugDrawList_Posteffects.connectTo(&visualizePosteffect_Distancefield_boundingSpheres_raymarch);

  connect(scene, &scene::Scene::sceneCleared, this, &Renderer::forceNewGridCameraPos);

  setAdjustRoughness(true);
  setSDFShadows(false);

  collectAmbientOcclusionToGrid[0] = nullptr;
  collectAmbientOcclusionToGrid[1] = &collectAmbientOcclusionToGrid1;
  collectAmbientOcclusionToGrid[2] = &collectAmbientOcclusionToGrid2;
  collectAmbientOcclusionToGrid[3] = &collectAmbientOcclusionToGrid3;

  initCascadedGridTextures();
}

Renderer::~Renderer()
{
  deinitCascadedGridTextures();
}

void Renderer::render()
{
  PROFILE_SCOPE("Renderer::render()")

  staticMeshRenderer.update(); // otherwise, the transformations don't get updated

  if(Q_UNLIKELY(needRerecording()))
    recordCommandlist();

  updateCameraUniform();

  if(isUsingBvhLeafGrid())
    updateBvhLeafGrid();

  voxelUniformBuffer.mergeStaticSDFs();

  prepareFramebuffer();

  commandList.call();

  debugDrawList_Posteffects.render();
  applyFramebuffer();

  sceneUniformBuffer.BindUniformBuffer(UNIFORM_BINDING_SCENE_BLOCK);
  debugDrawList_Lines.render();
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
  else if(pass == Pass::GBUFFER_FILL_PASS)
    preprocessorBlock.insert("#define GBUFFER_FILL_PASS");
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

void Renderer::initCascadedGridTextures()
{
  std::vector<uint8_t> r8ui_dummy_data;
  std::vector<uint16_t> r16ui_dummy_data;
  std::vector<uint16_t> rgba16ui_dummy_data;

  r8ui_dummy_data.resize(16*16*16);
  r16ui_dummy_data.resize(16*16*16);
  rgba16ui_dummy_data.resize(16*16*16*4);

  for(uint8_t u=0; u<16; ++u)
  {
    for(uint8_t v=0; v<16; ++v)
    {
      for(uint8_t w=0; w<16; ++w)
      {
        uint16_t is_even = (u%2) ^ (v%2) ^ (w%2);

        uint8_t r8ui_value = uint8_t(is_even * 255) + 0;
        uint16_t r16ui_value = uint16_t(is_even * 255) + 0;

        r8ui_dummy_data[(u*256 + v*16 + w) * 1] = r8ui_value;
        r16ui_dummy_data[(u*256 + v*16 + w) * 1] = r16ui_value;
        rgba16ui_dummy_data[(u*256 + v*16 + w) * 4 + 0] = r16ui_value;
        rgba16ui_dummy_data[(u*256 + v*16 + w) * 4 + 1] = r16ui_value/2;
        rgba16ui_dummy_data[(u*256 + v*16 + w) * 4 + 2] = r16ui_value/3;
        rgba16ui_dummy_data[(u*256 + v*16 + w) * 4 + 3] = r16ui_value/4;
      }
    }
  }

  auto textureFormat = [](int i) {
    GlTexture::Format f;
    if(i<MAX_NUM_GRID_CASCADES)
      f = GlTexture::Format::RED_INTEGER;
    else
      f = GlTexture::Format::RGBA_INTEGER;

    return GlTexture::format(glm::uvec3(16, 16, 16), 0, f, GlTexture::Type::UINT16, GlTexture::Target::TEXTURE_3D);
  };
  auto imageFormat = [](int i) -> GLenum {
    if(i<MAX_NUM_GRID_CASCADES)
      return GL_R16UI;
    else
      return GL_RGBA16UI;
  };
  auto init_data = [&](int i) -> const uint16_t* {
    if(i<MAX_NUM_GRID_CASCADES)
      return r16ui_dummy_data.data();
    else
      return rgba16ui_dummy_data.data();
  };

  for(int i=0; i<MAX_NUM_GRID_CASCADES*2; ++i)
  {
    gridTexture[i].setUncompressed2DImage(textureFormat(i), init_data(i));
    gridTexture[i].makeComplete();
    gl::TextureId textureId = gridTexture[i].textureId;

    GLuint64 imageHandle = GL_RET_CALL(glGetImageHandleNV, textureId, 0, GL_TRUE, 0, imageFormat(i));
    GLuint64 textureHandle = GL_RET_CALL(glGetTextureHandleNV, textureId);

    GL_CALL(glMakeTextureHandleResidentNV, textureHandle);
    Q_ASSERT(GL_RET_CALL(glIsTextureHandleResidentNV, textureHandle));

#if !(RESIDENCY_TACTIC&MAKE_IMAGE_ONLY_RESIDENT_IF_NECESSARY)
    GL_CALL(glMakeImageHandleResidentNV, imageHandle, GL_WRITE_ONLY);
    Q_ASSERT(GL_RET_CALL(glIsImageHandleResidentNV, imageHandle));
#endif

    computeTextureHandles[i] = imageHandle;
    renderTextureHandles[i] = textureHandle;
  }

  const gl::SamplerObject& samplerObject = gl::SamplerObject::GetSamplerObject(gl::SamplerObject::Desc(gl::SamplerObject::Filter::NEAREST,
                                                                                                       gl::SamplerObject::Filter::LINEAR,
                                                                                                       gl::SamplerObject::Filter::NEAREST,
                                                                                                       gl::SamplerObject::Border::CLAMP,
                                                                                                       1 /* max anisotropy */,
                                                                                                       glm::vec4(0.5f),
                                                                                                       gl::SamplerObject::CompareMode::NONE,
                                                                                                       0.f /* min lod */,
                                                                                                       0.f /* max lod */));
  GLuint clampedLinearSampler = samplerObject.GetInternHandle();

  for(int i=0; i<MAX_NUM_GRID_CASCADES; ++i)
  {
    gridOcclusionTexture[i].setUncompressed2DImage(GlTexture::format(glm::uvec3(16, 16, 16), 0, GlTexture::Format::RED, GlTexture::Type::UINT8, GlTexture::Target::TEXTURE_3D), r8ui_dummy_data.data());
    gl::TextureId textureId = gridOcclusionTexture[i].textureId;
    // Make the texture complete

    GL_CALL(glTextureParameteri, textureId, GL_TEXTURE_BASE_LEVEL, 0);
    GL_CALL(glTextureParameteri, textureId, GL_TEXTURE_MAX_LEVEL, 0);

    GLuint64 imageHandle = GL_RET_CALL(glGetImageHandleNV, textureId, 0, GL_TRUE, 0, GL_R8);
    GLuint64 textureHandle = GL_RET_CALL(glGetTextureSamplerHandleNV, textureId, clampedLinearSampler);

    GL_CALL(glMakeTextureHandleResidentNV, textureHandle);
    Q_ASSERT(GL_RET_CALL(glIsTextureHandleResidentNV, textureHandle));

#if !(RESIDENCY_TACTIC&MAKE_IMAGE_ONLY_RESIDENT_IF_NECESSARY)
    GL_CALL(glMakeImageHandleResidentNV, imageHandle, GL_WRITE_ONLY);
    Q_ASSERT(GL_RET_CALL(glIsImageHandleResidentNV, imageHandle));
#endif

    computeOcclusionTextureHandles[i] = imageHandle;
    renderOcclusionTextureHandles[i] = textureHandle;
  }
}

void Renderer::deinitCascadedGridTextures()
{
}

inline Renderer::CascadedGridsHeader Renderer::updateCascadedGrids() const
{
  PROFILE_SCOPE("Renderer::updateCascadedGrids()")

  float gridSizes[3] = {8., 16, 32};
  gridSizes[num_grid_cascades()-1] = 32;
  gridSizes[0] = 8;

  CascadedGridsHeader header;

  float margin = 1.f;

  for(int i=0; i<num_grid_cascades(); ++i)
  {
    float gridSize = gridSizes[i];

    const int numGridCells = 16;

    float grid_scale_factor = numGridCells / gridSize;

    // See /doc/cascaded_grid_position.svg
    float d_aa = numGridCells*0.5f - (margin + 1);
    d_aa /= grid_scale_factor;
    glm::vec3 grid_center = grid_camera_pos + d_aa * grid_camera_dir;

    glm::vec3 snapped_grid_center = glm::roundMultiple(grid_center, glm::vec3(1.f / grid_scale_factor));

    glm::vec3 grid_origin = grid_center - gridSize*0.5f;
    glm::vec3 snapped_grid_origin = snapped_grid_center - gridSize*0.5f;

    header.snappedGridLocation[i] = glm::vec4(snapped_grid_origin, grid_scale_factor);
    header.smoothGridLocation[i] = glm::vec4(grid_origin, grid_scale_factor);
  }

  int texture_base = bvh_is_grid_with_four_components(renderer::currentBvhUsage) ? MAX_NUM_GRID_CASCADES : 0;

  for(int i=0; i<num_grid_cascades(); ++i)
  {
    header.gridTexture[i] = renderTextureHandles[i + texture_base];
    header.occlusionTexture[i] = renderOcclusionTextureHandles[i];
  }

  return header;
}

void Renderer::updateBvhLeafGrid()
{
  PROFILE_SCOPE("Renderer::updateBvhLeafGrid()")

  const int texture_base = bvh_is_grid_with_four_components(renderer::currentBvhUsage) ? MAX_NUM_GRID_CASCADES : 0;
  for(int i=0; i<num_grid_cascades(); ++i)
  {
#if RESIDENCY_TACTIC&MAKE_TEXTURE_NON_RESIDENT_BEFORE_COMPUTING
    GL_CALL(glMakeTextureHandleNonResidentNV, renderTextureHandles[i + texture_base]);
    GL_CALL(glMakeTextureHandleNonResidentNV, renderOcclusionTextureHandles[i]);
#endif

#if RESIDENCY_TACTIC&MAKE_IMAGE_ONLY_RESIDENT_IF_NECESSARY
    GL_CALL(glMakeImageHandleResidentNV, computeTextureHandles[i + texture_base], GL_WRITE_ONLY);
    GL_CALL(glMakeImageHandleResidentNV, computeOcclusionTextureHandles[i], GL_WRITE_ONLY);
#endif

    Q_ASSERT(GL_RET_CALL(glIsImageHandleResidentNV, computeTextureHandles[i + texture_base]));
    Q_ASSERT(GL_RET_CALL(glIsImageHandleResidentNV, computeOcclusionTextureHandles[i]));
  }

  sceneUniformBuffer.BindUniformBuffer(UNIFORM_BINDING_SCENE_BLOCK);
  aoCollectHeaderUniformBuffer.BindUniformBuffer(UNIFORM_COLLECT_OCCLUSION_METADATA_BLOCK);

  Q_ASSERT(collectAmbientOcclusionToGrid[num_grid_cascades()] != nullptr);
  collectAmbientOcclusionToGrid[num_grid_cascades()]->invoke();

  for(int i=0; i<num_grid_cascades(); ++i)
  {
#if RESIDENCY_TACTIC&MAKE_IMAGE_ONLY_RESIDENT_IF_NECESSARY
    GL_CALL(glMakeImageHandleNonResidentNV, computeTextureHandles[i + texture_base]);
    GL_CALL(glMakeImageHandleNonResidentNV, computeOcclusionTextureHandles[i]);
#endif

#if RESIDENCY_TACTIC&MAKE_TEXTURE_NON_RESIDENT_BEFORE_COMPUTING
    GL_CALL(glMakeTextureHandleResidentNV, renderTextureHandles[i + texture_base]);
    GL_CALL(glMakeTextureHandleResidentNV, renderOcclusionTextureHandles[i]);
#endif

    Q_ASSERT(GL_RET_CALL(glIsTextureHandleResidentNV, renderTextureHandles[i + texture_base]));
    Q_ASSERT(GL_RET_CALL(glIsTextureHandleResidentNV, renderOcclusionTextureHandles[i]));
  }
}

void Renderer::update_aoCollectHeader()
{
  if(isUsingBvhLeafGrid())
  {
    AoCollectHeader& aoCollectSceneUniformData = *reinterpret_cast<AoCollectHeader*>(aoCollectHeaderUniformBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));

    int texture_base = bvh_is_grid_with_four_components(renderer::currentBvhUsage) ? num_grid_cascades() : 0;
    for(int i=0; i<num_grid_cascades(); ++i)
    {
      aoCollectSceneUniformData.gridTexture[i] = computeTextureHandles[i + texture_base];
      aoCollectSceneUniformData.occlusionTexture[i] = computeOcclusionTextureHandles[i];
    }

    aoCollectHeaderUniformBuffer.Unmap();
  }
}

bool Renderer::needRecapturing() const
{
  return _needRecapturing;
}

bool Renderer::needRerecording() const
{
  return staticMeshRenderer.needRerecording() || _needRecapturing || lightUniformBuffer.needRerecording();
}

void Renderer::captureStates()
{
  for(MaterialState& materialState : materialStates)
  {
    Q_ASSERT(materialState.framebuffer != nullptr);

    gl::FramebufferObject& framebuffer = *materialState.framebuffer;
    ReloadableShader& shader = materialShaders[materialState.shader];

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
      glm::ivec2 range = recorder.endTokenList();
      recorder.append_drawcall(range, &materialShader.stateCapture, materialShader.framebuffer);
    }
  }
}

void Renderer::recordCommandlist()
{
  PROFILE_SCOPE("Renderer::recordCommandlist()")
  if(Q_UNLIKELY(needRecapturing()))
    captureStates();

  glm::ivec2 commonTokenList;
  gl::CommandListRecorder recorder;

  recorder.beginTokenList();
  debugPrinter.recordBinding(recorder);
  recorder.append_token_Viewport(glm::uvec2(0), glm::uvec2(videoResolution));
  recorder.append_token_UniformAddress(UNIFORM_BINDING_SCENE_BLOCK, gl::ShaderType::VERTEX, sceneUniformBuffer.gpuBufferAddress());
  recorder.append_token_UniformAddress(UNIFORM_BINDING_SCENE_BLOCK, gl::ShaderType::FRAGMENT, sceneUniformBuffer.gpuBufferAddress());
  commonTokenList = recorder.endTokenList();

  TokenRanges meshDrawRanges = staticMeshRenderer.recordCommandList(recorder, commonTokenList);
  QSet<Material::Type> unusedMaterialTypes = meshDrawRanges.keys().toSet();

  for(auto i=materialShaderMetadata.begin(); i!=materialShaderMetadata.end(); ++i)
  {
    Material::Type materialType = i.key().second;
    const MaterialState& materialShader = *i.value();

    unusedMaterialTypes.remove(materialType);

    glm::ivec2 range;

    recordLightVisualization(recorder, materialType, materialShader, commonTokenList);

    if(meshDrawRanges.contains(materialType))
    {
      range = meshDrawRanges[materialType];
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

  update_aoCollectHeader();
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
    if(!this->cameraComponent && !cameraComponents.isEmpty())
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

void Renderer::forceNewGridCameraPos()
{
  grid_camera_pos = glm::vec3(NAN);
}

void Renderer::fillCameraUniform(const scene::CameraParameter& cameraParameter)
{
  glm::vec3 camera_position = cameraParameter.position;
  glm::mat4 view_projection_matrix = cameraParameter.projectionMatrix() * cameraParameter.viewMatrix();

  if(Q_LIKELY(_update_grid_camera || glm::isnan(grid_camera_pos.x)))
  {
    grid_camera_pos = camera_position;
    grid_camera_dir = cameraParameter.lookAt;
  }

  PROFILE_SCOPE("Renderer::fillCameraUniform()")

  SceneUniformBlock& sceneUniformData =  *reinterpret_cast<SceneUniformBlock*>(sceneUniformBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  sceneUniformData.camera_position = camera_position;
  sceneUniformData.view_projection_matrix = view_projection_matrix;
  sceneUniformData.lightData = lightUniformBuffer.updateLightData();
  sceneUniformData.voxelHeader = voxelUniformBuffer.updateVoxelHeader();
  sceneUniformData.totalTime = debugPosteffect.totalTime;
  sceneUniformData.costsHeatvisionBlackLevel = costsHeatvisionBlackLevel;
  sceneUniformData.costsHeatvisionWhiteLevel = costsHeatvisionWhiteLevel;
  sceneUniformData.bvh_debug_depth_begin = bvh_debug_depth_begin;
  sceneUniformData.bvh_debug_depth_end = bvh_debug_depth_end;
  sceneUniformData.candidateGrid = voxelUniformBuffer.candidateGridHeader;
  sceneUniformData.cascadedGrids = updateCascadedGrids();
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

bool Renderer::update_grid_camera() const
{
  return _update_grid_camera;
}

void Renderer::set_update_grid_camera(bool update_grid_camera)
{
  _update_grid_camera = update_grid_camera;
}

} // namespace renderer
} // namespace glrt

