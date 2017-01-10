#include <glrt/toolkit/profiler.h>
#include <glrt/application.h>
#include <glrt/gui/anttweakbar.h>
#include <glrt/renderer/bvh-usage.h>
#include <glrt/renderer/scene-renderer.h>
#include <glrt/renderer/debugging/shader-debug-printer.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>
#include <glrt/renderer/debugging/visualization-renderer.h>

namespace glrt {
namespace gui {


AntTweakBar::AntTweakBar(Application* application, const Settings& settings)
  : application(application),
    visible(settings.showByDefault),
    toggleHelp(settings.toggleHelp),
    toggleGui(settings.toggleGui),
    toggleLogHeatVision_debugPosteffect("LOG_HEATVISION_DEBUG_POSTEFFECT"),
    toggleLogHeatVision_costs("LOG_HEATVISION_DEBUG_COSTS"),
    toggleLogHeatVisionColors("HEATVISION_COLORS"),
    toggleDistancefieldFixedSamplePoints("DISTANCEFIELD_FIXED_SAMPLE_POINTS"),
    toggleDistancefieldAOSphereTracing("DISTANCEFIELD_AO_SPHERE_TRACING"),
    toggleConeBouquetNoise("CONE_BOUQUET_NOISE"),
    toggleConeBouquetUnderwaterCaustics("CONE_BOUQUET_UNDERWATER_CAUSICS")
{
  toggleLogHeatVision_debugPosteffect.setter(true);
  toggleLogHeatVision_costs.setter(false);
  toggleLogHeatVisionColors.setter(true);
  toggleDistancefieldFixedSamplePoints.setter(false);
  toggleDistancefieldAOSphereTracing.setter(true);
  toggleConeBouquetNoise.setter(false);
  toggleConeBouquetUnderwaterCaustics.setter(false);

  Q_ASSERT(application != nullptr);

  TwInit(TW_OPENGL_CORE, NULL);
  updateAntTweakBarWindowSize();

  // Prevent bars to get outside the render window
  TwSetParam(nullptr, nullptr, "contained", TW_PARAM_CSTRING, 1, "true");

  if(!settings.globalDescription.isEmpty())
    TwSetParam(nullptr, nullptr, "help", TW_PARAM_CSTRING, 1, settings.globalDescription.toStdString().c_str());

  toolbar.init(application);
}


AntTweakBar::~AntTweakBar()
{
  cameraSwitcher.clear();
  sceneSwitcher.clear();
  visualizationSwitcher.clear();
  bvhUsageSwitcher.clear();

  toolbar.deinit();
  TwTerminate();

}


TwBar* AntTweakBar::createCustomBar(QString name,
                                    QString help,
                                    glm::ivec2 pos,
                                    glm::ivec2 size,
                                    int marginToWindowBorder)
{
  // the defautlt margin (32 px) is exactly the radius of the RotoSlider.
  // By introducing this margin, we are guaranteed to have some space to use
  // the RotoSlider.
  marginToWindowBorder = glm::max(marginToWindowBorder, 0);

  TwBar* tweakBar = TwNewBar(name.toStdString().c_str());
  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, help.toStdString().c_str());

  setTweaBarAllocation(tweakBar, pos, size, glm::ivec2(marginToWindowBorder));

  gui::Toolbar::registerTweakBar(tweakBar);

  return tweakBar;
}

void AntTweakBar::setTweaBarAllocation(TwBar* tweakBar, glm::ivec2 pos, glm::ivec2 size, const glm::ivec2& marginToWindowBorder)
{
  TwSetParam(tweakBar, nullptr, "size", TW_PARAM_INT32, 2, &size);

  glm::ivec2 windowSize;
  SDL_GetWindowSize(application->sdlWindow, &windowSize.x, &windowSize.y);
  TwGetParam(tweakBar, nullptr, "size", TW_PARAM_INT32, 2, &size);

  pos = glm::clamp(pos, glm::ivec2(marginToWindowBorder), windowSize-size-marginToWindowBorder);
  TwSetParam(tweakBar, nullptr, "position", TW_PARAM_INT32, 2, &pos);
}

TwBar* AntTweakBar::createProfilerBar(Profiler* profiler)
{
  TwBar* tweakBar = TwNewBar("Profiler");

  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, "Collection of tools to measure the performance.\nNote: For better Performance measurement, you can toggle AntTweakbar with [F9]");

  TwAddVarRW(tweakBar, "Print FPS", TW_TYPE_BOOLCPP, &profiler->printFramerate, "");
  TwSetParam(tweakBar, nullptr, "size", TW_PARAM_CSTRING, 1, "320 320");

  toggleProfiler.getter = [profiler]()->bool{return profiler->isActive();};
  toggleProfiler.setter = [profiler](bool a){
    if(a)
      profiler->activate();
    else
      profiler->deactivate();
  };
  toggleProfiler.TwAddVarCB(tweakBar, "Connect", "key=F10 help='Connect with the profiler gui (which must be already running)'");

  gui::Toolbar::registerTweakBar(tweakBar, true);

  return tweakBar;
}


TwBar* AntTweakBar::createDebugSceneBar(renderer::Renderer* renderer)
{
  scene::Scene& scene = renderer->scene;

  connect(&scene, &scene::Scene::sceneLoadedExt, this, &AntTweakBar::handleSceneLoaded);

  TwBar* tweakBar = TwNewBar("Scene");

  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, "Collection of tools to debug a scene.");
  TwSetParam(tweakBar, nullptr, "size", TW_PARAM_CSTRING, 1, "320 320");

  sceneSwitcher = SceneEnumeration::Ptr(new SceneEnumeration("CurrentSceneEnum", tweakBar, "Current Scene", ""));
  sceneSwitcher->initWithUuids<Scene>(scene.resourceManager, scene.resourceManager.allRegisteredScenes());
  sceneSwitcher->valueChangedByUser = [&scene](const Uuid<Scene>& uuid){scene.load(uuid);};

  renderer->visualizePosteffect_Distancefield_boundingSpheres_raymarch.guiToggle.TwAddVarCB(tweakBar, "SDF B-Spheres", "group='Debug Voxels' help='Show the Bounding Spheres of the Distancefield Components as if they would define the distance field'");
  renderer->visualizePosteffect_Distancefield_raymarch.guiToggle.TwAddVarCB(tweakBar, "Distance-Field Ray-March", "group='Debug Voxels'");
  renderer->visualizePosteffect_Fallback_Distancefield_raymarch.guiToggle.TwAddVarCB(tweakBar, "Fallback Distance-Field Ray-March", "group='Debug Voxels'");
  renderer->visualizePosteffect_Voxel_Cubic_raymarch.guiToggle.TwAddVarCB(tweakBar, "Cubic-Voxel Ray-March", "group='Debug Voxels'");
  renderer->visualizePosteffect_Voxel_BoundingBox.guiToggle.TwAddVarCB(tweakBar, "Highlight Voxel BoundingBox", "group='Debug Voxels'");
  renderer->visualizePosteffect_Voxel_HighlightUnconveiledNegativeDistances.guiToggle.TwAddVarCB(tweakBar, "Highlight Unconveiled Negative Distances", "group='Debug Voxels'");
  TwSetParam(tweakBar, "Debug Voxels", "opened", TW_PARAM_CSTRING, 1, "false");

  TwAddVarRW(tweakBar, "Clear Framebuffer", TW_TYPE_BOOLCPP, &renderer->debugDrawList_Posteffects.clearBuffer, "group='Debug Shader'");
  renderer->visualizePosteffect_OrangeTest.guiToggle.TwAddVarCB(tweakBar, "Orange CommandList Test", "group='Debug Shader'");
  TwAddVarRW(tweakBar, "Enable Lighting in Debug", TW_TYPE_BOOL32, &renderer->debugPosteffect.useLighting, "group='Debug Shader'");
  TwAddVarRW(tweakBar, "Enable Directional Lighting in Debug", TW_TYPE_BOOL32, &renderer->debugPosteffect.useDirectionalLighting, "group='Debug Shader'");
  TwAddVarRW(tweakBar, "Show WorldPos in Debug", TW_TYPE_BOOL32, &renderer->debugPosteffect.showWorldPos, "group='Debug Shader'");
  TwAddVarRW(tweakBar, "Show Normals in Debug", TW_TYPE_BOOL32, &renderer->debugPosteffect.showNormals, "group='Debug Shader'");
  TwAddVarRW(tweakBar, "Distance-Field Offset", TW_TYPE_FLOAT, &renderer->debugPosteffect.distanceFieldOffset, "group='Debug Shader' precision=3 min=-32 max=32 step=0.001");
  TwAddVarRW(tweakBar, "Show", TW_TYPE_BOOL32, &renderer->debugPosteffect.showNumSteps, "group='Debug Shader/Show Step-Count'");
  TwAddVarRW(tweakBar, "Black-Level", TW_TYPE_INT32, &renderer->debugPosteffect.stepCountAsBlack, "group='Debug Shader/Show Step-Count' min=0 max=2147483647");
  TwAddVarRW(tweakBar, "White-Level", TW_TYPE_INT32, &renderer->debugPosteffect.stepCountAsWhite, "group='Debug Shader/Show Step-Count' min=1 max=2147483647");
  toggleLogHeatVision_debugPosteffect.TwAddVarCB(tweakBar, "Logarithmic", "group='Debug Shader/Show Step-Count'");
  toggleLogHeatVisionColors.TwAddVarCB(tweakBar, "Colors", "group='Debug Shader/Show Step-Count'");
  TwSetParam(tweakBar, "Debug Shader", "opened", TW_PARAM_CSTRING, 1, "false");
  TwSetParam(tweakBar, "Debug Shader/Show Step-Count", "opened", TW_PARAM_CSTRING, 1, "false");

  //-------- Camera ------------------------------------------------------------
  cameraSwitcher = CameraEnumeration::Ptr(new CameraEnumeration("CurrentCameraEnum", tweakBar, "Current Camera", "group=Camera"));
  cameraSwitcher->valueChanged = [&scene](const QPointer<scene::CameraComponent>& otherCamera){switchDebugCameraTo(&scene, otherCamera);};
  TwAddVarRW(tweakBar, "Lock Camera", TW_TYPE_BOOLCPP, &scene::FpsDebugInputHandler::locked, "group=Camera");
  TwSetParam(tweakBar, "Camera", "opened", TW_PARAM_CSTRING, 1, "false");

  //-------- Debug Scene -------------------------------------------------------
  disableSceneryVoxels.setter = [renderer,this,&scene](bool v){
    this->_disableSceneryVoxels=v;
    Array<scene::VoxelDataComponent*> voxelComponents = scene::collectAllComponentsWithType<scene::VoxelDataComponent>(&scene,
                                                                                                                       [](scene::VoxelDataComponent* c){
                                                        return c->voxelizedAsScenery();
    });

    for(scene::VoxelDataComponent* c : voxelComponents)
      c->setVisible(!v);
  };
  disableSceneryVoxels.getter = [this]() -> bool {return _disableSceneryVoxels;};
  disableSceneryVoxels.TwAddVarCB(tweakBar, "DisableSceneryVoxels", "group='Debug Scene'");

  renderer->visualizeVoxelGrids.guiToggle.TwAddVarCB(tweakBar, "Show VoxelGrids", "group='Debug Scene'");
  renderer->visualizeVoxelBoundingSpheres.guiToggle.TwAddVarCB(tweakBar, "Show SDF B-Spheres", "group='Debug Scene' help='Show the Bounding Spheres of the Distancefield Components'");
  renderer->visualizeBVH.guiToggle.TwAddVarCB(tweakBar, "Show BVH Tree", "group='Debug Scene' help='Show the SDF Bounding Volume Hierarchy'");

  TwAddVarRW(tweakBar, "BVH Debug depth begin", TW_TYPE_UINT16, &renderer->bvh_debug_depth_begin, QString("group='Debug Scene' min=0 max=%0").arg(renderer::MAX_NUM_STATIC_MESHES).toStdString().c_str());
  TwAddVarRW(tweakBar, "BVH Debug depth end", TW_TYPE_UINT16, &renderer->bvh_debug_depth_end, QString("group='Debug Scene' min=1 max=%0").arg(renderer::MAX_NUM_STATIC_MESHES).toStdString().c_str());
  renderer->visualizeBVH_Grid.guiToggle.TwAddVarCB(tweakBar, "Show BVH Cascaded Grids", "group='Debug Scene' help='Show the Grids accessing the SDF Bounding Volume Hierarchy'");

  renderer->visualizeSdfCandidateGrid.guiToggle.TwAddVarCB(tweakBar, "Show SDF Candidate-Grid", "group='Debug Scene'");
  renderer->visualizeSdfCandidateCell.guiToggle.TwAddVarCB(tweakBar, "Show SDF Candidate-Cell", "group='Debug Scene'");
  currentSdfCellToDebug_x.TwAddVarCB(tweakBar, "cell_x", "group='Debug Scene' visible=false min=0 max=31");
  currentSdfCellToDebug_y.TwAddVarCB(tweakBar, "cell_y", "group='Debug Scene' visible=false min=0 max=31");
  currentSdfCellToDebug_z.TwAddVarCB(tweakBar, "cell_z", "group='Debug Scene' visible=false min=0 max=31");
  currentSdfCellToDebug_x.getter = []() -> int {return glm::min<int>(renderer::debugging::VisualizationRenderer::selectedSdfCandidateGrid.x, int(renderer::SDF_CANDIDATE_GRID_SIZE)-1);};
  currentSdfCellToDebug_y.getter = []() -> int {return glm::min<int>(renderer::debugging::VisualizationRenderer::selectedSdfCandidateGrid.y, int(renderer::SDF_CANDIDATE_GRID_SIZE)-1);};
  currentSdfCellToDebug_z.getter = []() -> int {return glm::min<int>(renderer::debugging::VisualizationRenderer::selectedSdfCandidateGrid.z, int(renderer::SDF_CANDIDATE_GRID_SIZE)-1);};
  currentSdfCellToDebug_x.setter = [renderer](int v){renderer::debugging::VisualizationRenderer::selectedSdfCandidateGrid.x = glm::min<int>(v, int(renderer::SDF_CANDIDATE_GRID_SIZE)-1);renderer->visualizeSdfCandidateCell.reinit();};
  currentSdfCellToDebug_y.setter = [renderer](int v){renderer::debugging::VisualizationRenderer::selectedSdfCandidateGrid.y = glm::min<int>(v, int(renderer::SDF_CANDIDATE_GRID_SIZE)-1);renderer->visualizeSdfCandidateCell.reinit();};
  currentSdfCellToDebug_z.setter = [renderer](int v){renderer::debugging::VisualizationRenderer::selectedSdfCandidateGrid.z = glm::min<int>(v, int(renderer::SDF_CANDIDATE_GRID_SIZE)-1);renderer->visualizeSdfCandidateCell.reinit();};
  renderer->visualizeSdfFallbackGrid.guiToggle.TwAddVarCB(tweakBar, "Show SDF Fallback-Grid", "group='Debug Scene'");

  std::function<void(bool)> setSdfCandidateEnabled = renderer->visualizeSdfCandidateCell.guiToggle.setter;
  renderer->visualizeSdfCandidateCell.guiToggle.setter = [setSdfCandidateEnabled,this](bool b) {
    currentSdfCellToDebug_x.setVisible(b);
    currentSdfCellToDebug_y.setVisible(b);
    currentSdfCellToDebug_z.setVisible(b);
    setSdfCandidateEnabled(b);
  };

  renderer->visualizeBoundingBoxes.guiToggle.TwAddVarCB(tweakBar, "Show Voxel-AABBs", "group='Debug Scene'");
  renderer->visualizeSceneBoundingBox.guiToggle.TwAddVarCB(tweakBar, "Show Scene-AABB", "group='Debug Scene'");

  renderer->visualizeCameras.guiToggle.TwAddVarCB(tweakBar, "Show Scene Cameras", "group='Debug Scene'");
  updateGridCameraToggle.setter = [renderer](bool ar){renderer->set_update_grid_camera(ar);};
  updateGridCameraToggle.getter = [renderer]() -> bool {return renderer->update_grid_camera();};
  updateGridCameraToggle.TwAddVarCB(tweakBar, "Update Grid for Camera", "group='Debug Scene'");

  renderer->visualizeWorldGrid.guiToggle.TwAddVarCB(tweakBar, "Show World Grid", "group='Debug Scene'");
  renderer->visualizeUniformTest.guiToggle.TwAddVarCB(tweakBar, "Show Uniform Test", "group='Debug Scene'");

  renderer->visualizeSphereAreaLights.guiToggle.TwAddVarCB(tweakBar, "Show Sphere Area-Lights", "group='Debug Scene'");
  renderer->visualizeRectAreaLights.guiToggle.TwAddVarCB(tweakBar, "Show Rect Area-Lights", "group='Debug Scene'");
  TwSetParam(tweakBar, "Debug Scene", "opened", TW_PARAM_CSTRING, 1, "true");

  //-------- Scene Statistics---------------------------------------------------
  TwAddVarRO(tweakBar, "Num Visible SDF instances", TW_TYPE_UINT32, &renderer->statistics.numSdfInstances, "group='Statistics'");
  TwSetParam(tweakBar, "Statistics", "opened", TW_PARAM_CSTRING, 1, "false");


  gui::Toolbar::registerTweakBar(tweakBar, true);

  return tweakBar;
}


void __reload_all_shaders(void*)
{
  renderer::ReloadableShader::reloadAll();
}


TwBar* AntTweakBar::createDebugShaderBar(renderer::Renderer* renderer, renderer::debugging::ShaderDebugPrinter* shaderDebugPrinter)
{
  TwBar* tweakBar = TwNewBar("Shader");

  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, "Collection of tools to debug a shader.");
  setTweaBarAllocation(tweakBar, glm::ivec2(0, 4096), glm::ivec2(480, 160), glm::ivec2(32, 8));
  TwSetParam(tweakBar, nullptr, "valueswidth", TW_PARAM_CSTRING, 1, "256");

  visualizationSwitcher = VisualizationEnumeration::Ptr(new VisualizationEnumeration("VisualizationEnumeration", tweakBar, "Shader Visualization", "keyincr=F7 keydecr=SHIFT+F7 help='The Surface Shader vizualization'"));
  visualizationSwitcher->init(glrt::renderer::allSurfaceShaderVisualizations());
  visualizationSwitcher->setCurrentValue(glrt::renderer::currentSurfaceShaderVisualization);
  visualizationSwitcher->valueChanged = [](glrt::renderer::SurfaceShaderVisualization visualization){glrt::renderer::setCurrentSurfaceShaderVisualization(visualization);};




  TwAddButton(tweakBar, "Reload Shaders", __reload_all_shaders, nullptr, "key=F5 help='Reloads all reloadable shaders'");


  //-------- Optimization ---------------------------------------------------
  sortObjectsBySDF = glrt::scene::SORT_OBJECTS_BY_SDF_TEXTURE;
  sortObjectsBySDF.TwAddVarCB(tweakBar, "SortBySDF", "group='Optimization'");
  AO_INSTANCES_IN_INNER_LOOP = glrt::renderer::AO_INSTANCES_IN_INNER_LOOP;
  AO_INSTANCES_IN_INNER_LOOP.TwAddVarCB(tweakBar, "Instances in inner loop", "group='Optimization'");
  AO_USE_CANDIDATE_GRID = glrt::renderer::AO_USE_CANDIDATE_GRID;
  AO_USE_CANDIDATE_GRID.TwAddVarCB(tweakBar, "Use Candidate Grid", "group='Optimization'");

  AO_IGNORE_FALLBACK_SDF = glrt::renderer::AO_IGNORE_FALLBACK_SDF;
  AO_IGNORE_FALLBACK_SDF.TwAddVarCB(tweakBar, "AO ignore fallback texture", "group='Optimization'");

  AO_FALLBACK_SDF_ONLY = glrt::renderer::AO_FALLBACK_SDF_ONLY;
  AO_FALLBACK_SDF_ONLY.TwAddVarCB(tweakBar, "AO use only fallback texture", "group='Optimization'");

  MERGED_STATIC_SDF_SIZE = glrt::renderer::MERGED_STATIC_SDF_SIZE;
  MERGED_STATIC_SDF_SIZE.TwAddVarCB(tweakBar, "AO Fallback SDF size", "group='Optimization' min=16 max=512");

  AO_RADIUS = glrt::renderer::AO_RADIUS;
  AO_RADIUS.TwAddVarCB(tweakBar, "AO Radius", "group='Optimization' min=0.1 max=20");
  AO_STATIC_FALLBACK_FADING_START = glrt::renderer::AO_STATIC_FALLBACK_FADING_START;
  AO_STATIC_FALLBACK_FADING_START.TwAddVarCB(tweakBar, "AO Fallback Fading Start", "group='Optimization' min=0.1 max=20");
  AO_STATIC_FALLBACK_FADING_END = glrt::renderer::AO_STATIC_FALLBACK_FADING_END;
  AO_STATIC_FALLBACK_FADING_END.TwAddVarCB(tweakBar, "AO Fallback Fading End", "group='Optimization' min=0.1 max=20");
  AO_CANDIDATE_GRID_CONTAINS_INDICES = glrt::renderer::AO_CANDIDATE_GRID_CONTAINS_INDICES;
  AO_CANDIDATE_GRID_CONTAINS_INDICES.TwAddVarCB(tweakBar, "AO Candidate Grid Contains Indices", "group='Optimization'");

  TwSetParam(tweakBar, "Optimization", "opened", TW_PARAM_CSTRING, 1, "false");


  //-------- BVH---------------------------------------------------
  bvhUsageSwitcher = BvhUsageEnumeration::Ptr(new BvhUsageEnumeration("BvhUsageEnumeration", tweakBar, "BVH Usage", "keyincr=F8 keydecr=SHIFT+F8 help='Switch the suage of the BVH-tree' group='BVH'"));
  bvhUsageSwitcher->init(glrt::renderer::allcurrentBvhUsages());
  bvhUsageSwitcher->setCurrentValue(glrt::renderer::currentBvhUsage);
  bvhUsageSwitcher->valueChanged = [](glrt::renderer::BvhUsage bvhUsage){glrt::renderer::setCurrentBVHUsage(bvhUsage);};

  numBvhGrids = renderer::NUM_GRID_CASCADES;
  numBvhGrids.TwAddVarCB(tweakBar, "Num Grid Cascades", QString("min=1 max=%0 group='BVH'").arg(MAX_NUM_GRID_CASCADES).toStdString().c_str());

  bvhStackDepth = renderer::BVH_MAX_STACK_DEPTH;
  bvhStackDepth.TwAddVarCB(tweakBar, "Stack Depth", QString("min=1 max=%0 group='BVH'").arg(renderer::MAX_NUM_STATIC_MESHES).toStdString().c_str());

  bvhStackDepth.setter = [](uint16_t n){renderer::set_bvh_traversal_leaf_result_array_length(n);};
  bvhStackDepth.getter = []() -> uint16_t {return renderer::bvh_traversal_leaf_result_array_length();};
  bvhStackDepth.TwAddVarCB(tweakBar, "Num used Leaf Results ", QString("min=1 max=%0 group='BVH'").arg(renderer::MAX_NUM_STATIC_MESHES).toStdString().c_str());

  bvhEnforceHugeLeavesFirst = glrt::scene::ENFORCE_HUGE_BVH_LEAVES_FIRST;
  bvhEnforceHugeLeavesFirst.TwAddVarCB(tweakBar, "EnforceHugeLeavesFirst", "group='BVH'");

  TwSetParam(tweakBar, "BVH", "opened", TW_PARAM_CSTRING, 1, "false");

  //-------- PBS---------------------------------------------------
  roughnessAdjustmentToggle.setter = [renderer](bool ar){renderer->setAdjustRoughness(ar);};
  roughnessAdjustmentToggle.getter = [renderer]() -> bool {return renderer->adjustRoughness();};
  roughnessAdjustmentToggle.TwAddVarCB(tweakBar, "Roughness Adjustment", "group=PBS");
  TwSetParam(tweakBar, "PBS", "opened", TW_PARAM_CSTRING, 1, "false");

  //-------- Debug---------------------------------------------------
  sdfShadowsToggle.setter = [renderer](bool ar){renderer->setSDFShadows(ar);};
  sdfShadowsToggle.getter = [renderer]() -> bool {return renderer->sdfShadows();};
  sdfShadowsToggle.TwAddVarCB(tweakBar, "SDF Shadows", "group=Debug");

  sdfAOToggle.setter = [renderer](bool ar){renderer->setAmbientOcclusionSDF(ar);};
  sdfAOToggle.getter = [renderer]() -> bool {return renderer->ambientOcclusionSDF();};
  sdfAOToggle.TwAddVarCB(tweakBar, "SDF AO", "group=Debug");
  texAOToggle.setter = [renderer](bool ar){renderer->setAmbientOcclusionTexture(ar);};
  texAOToggle.getter = [renderer]() -> bool {return renderer->ambientOcclusionTexture();};
  texAOToggle.TwAddVarCB(tweakBar, "Use Texture AO", "group=Debug");
  iblDiffuseToggle.setter = [renderer](bool ar){renderer->setIBL_Diffuse(ar);};
  iblDiffuseToggle.getter = [renderer]() -> bool {return renderer->ibl_Diffuse();};
  iblDiffuseToggle.TwAddVarCB(tweakBar, "Use Diffuse IBL", "group=Debug");
  iblSpecularToggle.setter = [renderer](bool ar){renderer->setIBL_Specular(ar);};
  iblSpecularToggle.getter = [renderer]() -> bool {return renderer->ibl_Specular();};
  iblSpecularToggle.TwAddVarCB(tweakBar, "Use Specular IBL", "group=Debug");

  if(shaderDebugPrinter != nullptr)
  {
    shaderDebugPrinter->guiToggle.TwAddVarCB(tweakBar, "Use Printer", "group=Debug key=F6");
    TwAddVarRW(tweakBar, "Clear Scene", TW_TYPE_BOOLCPP, &shaderDebugPrinter->clearScene, "group=Debug");
  }
  gui::Toolbar::registerTweakBar(tweakBar, false);

  TwSetParam(tweakBar, "Debug", "opened", TW_PARAM_CSTRING, 1, "false");

  //-------- Debug/Show Costs ---------------------------------------------------
  TwAddVarRW(tweakBar, "Black-Level", TW_TYPE_INT32, &renderer->costsHeatvisionBlackLevel, "group='Debug/Show Costs' min=0 max=2147483647");
  TwAddVarRW(tweakBar, "White-Level", TW_TYPE_INT32, &renderer->costsHeatvisionWhiteLevel, "group='Debug/Show Costs' min=1 max=2147483647");
  toggleLogHeatVision_costs.TwAddVarCB(tweakBar, "Logarithmic", "group='Debug/Show Costs'");
  toggleLogHeatVisionColors.TwAddVarCB(tweakBar, "Colors", "group='Debug/Show Costs'");
  TwSetParam(tweakBar, "Debug/Show Costs", "opened", TW_PARAM_CSTRING, 1, "false");

  //-------- Debug/SDF-AO ---------------------------------------------------
  toggleDistancefieldFixedSamplePoints.TwAddVarCB(tweakBar, "Fixed-Sampling-Points", "group='Debug/SDF-AO'");
  toggleDistancefieldAOSphereTracing.TwAddVarCB(tweakBar, "Sphere-Tracing", "group='Debug/SDF-AO'");

  SDFSAMPLING_SPHERETRACING_START = renderer::SDFSAMPLING_SPHERETRACING_START;
  SDFSAMPLING_SELF_SHADOW_AVOIDANCE = renderer::SDFSAMPLING_SELF_SHADOW_AVOIDANCE;
  SDFSAMPLING_SPHERE_TRACING_MAX_NUM_LOOPS = renderer::SDFSAMPLING_SPHERE_TRACING_MAX_NUM_LOOPS;
  SDFSAMPLING_EXPONENTIAL_NUM = renderer::SDFSAMPLING_EXPONENTIAL_NUM;
  SDFSAMPLING_EXPONENTIAL_START = renderer::SDFSAMPLING_EXPONENTIAL_START;
  SDFSAMPLING_EXPONENTIAL_FIRST_SAMPLE = renderer::SDFSAMPLING_EXPONENTIAL_FIRST_SAMPLE;
  SDFSAMPLING_EXPONENTIAL_FACTOR = renderer::SDFSAMPLING_EXPONENTIAL_FACTOR;
  SDFSAMPLING_EXPONENTIAL_OFFSET = renderer::SDFSAMPLING_EXPONENTIAL_OFFSET;

  SDFSAMPLING_SPHERE_TRACING_MAX_NUM_LOOPS.TwAddVarCB(tweakBar, "SDFSAMPLING_SPHERE_TRACING_MAX_NUM_LOOPS", "group='Debug/SDF-AO' min=1 max=256");
  SDFSAMPLING_SPHERETRACING_START.TwAddVarCB(tweakBar, "SDFSAMPLING_SPHERETRACING_START", "group='Debug/SDF-AO' min=0 max=4 precision=3");
  SDFSAMPLING_SELF_SHADOW_AVOIDANCE.TwAddVarCB(tweakBar, "SDFSAMPLING_SELF_SHADOW_AVOIDANCE", "group='Debug/SDF-AO' min=0 max=2 precision=3");
  SDFSAMPLING_EXPONENTIAL_NUM.TwAddVarCB(tweakBar, "SDFSAMPLING_EXPONENTIAL_NUM", "group='Debug/SDF-AO' min=1 max=16");
  SDFSAMPLING_EXPONENTIAL_START.TwAddVarCB(tweakBar, "SDFSAMPLING_EXPONENTIAL_START", "group='Debug/SDF-AO' min=0 max=4 precision=3");
  SDFSAMPLING_EXPONENTIAL_FIRST_SAMPLE.TwAddVarCB(tweakBar, "SDFSAMPLING_EXPONENTIAL_FIRST_SAMPLE", "group='Debug/SDF-AO' min=0 max=1 precision=3");
  SDFSAMPLING_EXPONENTIAL_FACTOR.TwAddVarCB(tweakBar, "SDFSAMPLING_EXPONENTIAL_FACTOR", "group='Debug/SDF-AO' min=1 max=16 precision=3");
  SDFSAMPLING_EXPONENTIAL_OFFSET.TwAddVarCB(tweakBar, "SDFSAMPLING_EXPONENTIAL_OFFSET", "group='Debug/SDF-AO' min=0 max=1 precision=3");

  TwSetParam(tweakBar, "Debug/SDF-AO", "opened", TW_PARAM_CSTRING, 1, "false");

  //-------- Debug/Cone-Bouquet---------------------------------------------------
  toggleConeBouquetNoise.TwAddVarCB(tweakBar, "Noise", "group='Debug/Cone-Bouquet'");
  toggleConeBouquetUnderwaterCaustics.TwAddVarCB(tweakBar, "Underwater Caustics", "group='Debug/Cone-Bouquet'");
  TwSetParam(tweakBar, "Debug/Cone-Bouquet", "opened", TW_PARAM_CSTRING, 1, "false");

  return tweakBar;
}

void AntTweakBar::switchDebugCameraTo(Scene* scene, const QPointer<scene::CameraComponent>& otherCamera)
{
  QPointer<scene::CameraComponent> debugCamera = scene::findDebugCameraComponent(scene);
  scene::FpsDebugController* fpsController = scene::findFpsDebugController(scene);

  if(!debugCamera.isNull() && !otherCamera.isNull())
  {
    debugCamera->set_localCoordFrame(otherCamera->localCoordFrame());
    debugCamera->cameraParameter = otherCamera->cameraParameter;

    if(fpsController)
      fpsController->inputHandler.frame = otherCamera->localCoordFrame();
  }
}

void AntTweakBar::handleSceneLoaded(scene::Scene* scene)
{
  if(sceneSwitcher)
    sceneSwitcher->setCurrentValue(scene->uuid);

  if(cameraSwitcher)
  {
    QMap<QString, QPointer<scene::CameraComponent>> cameras;
    QHash<QString, scene::CameraComponent*> h = glrt::scene::collectAllComponentsWithTypeNamed<scene::CameraComponent>(scene);
    for(auto i=h.begin(); i!=h.end(); ++i)
    {
      if(i.value()->uuid != scene::uuids::debugCameraComponent)
        cameras.insert(i.key(), i.value());
    }
    cameraSwitcher->init(cameras);
    if(!cameras.isEmpty())
      cameraSwitcher->setCurrentKey(cameras.keys().first());

    QPointer<scene::CameraComponent> cameraComponent = glrt::scene::findComponent(scene, scene->camera(glrt::scene::CameraSlot::MAIN_CAMERA));
    if(!cameraComponent.isNull())
      cameraSwitcher->setCurrentValue(cameraComponent);

  }

  if(_disableSceneryVoxels && disableSceneryVoxels.setter)
    disableSceneryVoxels.setter(true);
}


bool AntTweakBar::handleEvents(const SDL_Event& event)
{
  PROFILE_SCOPE("AntTweakBar (Events)")

  if(visible && TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION))
  {
    handeledEvent(event);
    return true;
  }else
  {
    return unhandeledEvent(event);
  }
}


void AntTweakBar::handeledEvent(const SDL_Event& event)
{
  switch(event.type)
  {
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    if(event.button.button == SDL_BUTTON_LEFT)
    {
      bool captured_mouse = event.button.state==SDL_PRESSED;
      // Workaround:
      // Don't capture the mouse immediatly, as if the application freezes,
      // (because of an error while handling a pressed button),
      // the mouse should not be captured so the user can use other applications
      // As the event is lgging behind, wait 20 frames before capturing the mouse
      captureMouseDeferred = captured_mouse ? MouseCaptureState(20) : MouseCaptureState::IDLE;
      SDL_SetWindowGrab(application->sdlWindow, SDL_FALSE);
    }
    return;
  default:
    return;
  }
}


bool AntTweakBar::unhandeledEvent(const SDL_Event& event)
{
  switch(event.type)
  {
  case SDL_MOUSEBUTTONUP:
    if(event.button.button == SDL_BUTTON_LEFT)
      SDL_SetWindowGrab(application->sdlWindow, SDL_FALSE);
    return false;
  case SDL_KEYDOWN:
    switch(event.key.keysym.sym)
    {
    case SDLK_F1:
      if(toggleHelp)
      {
        TwBar* helpBar = TwGetBarByName("TW_HELP");
        qint32 iconified;

        Q_ASSERT(helpBar != nullptr);

        if(visible)
          TwGetParam(helpBar, nullptr, "iconified", TW_PARAM_INT32, 1, &iconified);
        else
          iconified = true;

        iconified = !iconified;

        TwSetParam(helpBar, nullptr, "iconified", TW_PARAM_INT32, 1, &iconified);
        updateAntTweakBarWindowSize();
        return true;
      }
      return false;
    case SDLK_F9:
      if(toggleGui)
      {
        this->visible = !this->visible;
        updateAntTweakBarWindowSize();
        return true;
      }
      return false;
    default:
      return false;
    }

  default:
    return false;
  }
}

// The mouse wasn't captured immediatly, as if the application freezes, because of an error while handling a pressed button, the mouse should not be captured during debugging
inline void AntTweakBar::deferredMouseCapture()
{
  if(captureMouseDeferred==MouseCaptureState::CAPTURE_NOW && this->visible)
  {
    captureMouseDeferred = MouseCaptureState::IDLE;
    SDL_SetWindowGrab(application->sdlWindow, SDL_TRUE);
  }else if(captureMouseDeferred>=MouseCaptureState::CAPTURE_NEXT_FRAME)
  {
    captureMouseDeferred = MouseCaptureState(int(captureMouseDeferred)-1);
  }
}


void AntTweakBar::draw()
{
  PROFILE_SCOPE("AntTweakBar (Draw)")

  if(visible)
    TwDraw();

  deferredMouseCapture();
}


void AntTweakBar::updateAntTweakBarWindowSize()
{
  glm::ivec2 size;

  SDL_GetWindowSize(application->sdlWindow, &size.x, &size.y);

  TwWindowSize(size.x,
               size.y);
}


TweakBarShaderToggle::TweakBarShaderToggle(const QString& macroName)
{
  getter = [macroName]() -> bool {
           return glrt::renderer::ReloadableShader::isMacroDefined(macroName);
  };
  setter = [macroName](bool value) {
           glrt::renderer::ReloadableShader::defineMacro(macroName, value);
  };
}


} // namespace gui
} // namespace glrt
