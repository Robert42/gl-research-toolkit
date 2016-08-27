#ifndef GLRT_RENDERER_RENDERER_H
#define GLRT_RENDERER_RENDERER_H


#include <glrt/scene/scene.h>
#include <glrt/scene/camera-component.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/resources/material.h>
#include <glrt/scene/component-decorator.h>
#include <glrt/renderer/declarations.h>
#include <glrt/renderer/debugging/visualization-renderer.h>
#include <glrt/renderer/static-mesh-buffer-manager.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>
#include <glrt/renderer/static-mesh-renderer.h>
#include <glrt/renderer/declarations.h>
#include <glrt/renderer/light-buffer.h>
#include <glrt/renderer/voxel-buffer.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/renderer/material-state.h>
#include <glrt/renderer/debugging/shader-debug-printer.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>
#include <glrt/renderer/compute-step.h>

#include <glhelper/framebufferobject.hpp>
#include <glhelper/texture2d.hpp>


namespace glrt {
namespace renderer {


class Renderer : public QObject, public ReloadableShader::Listener
{
  Q_OBJECT
public:
  struct Satistics
  {
    quint32 numSdfInstances = -1;
  };

  Satistics statistics;

  scene::Scene& scene;
  StaticMeshBufferManager& staticMeshBufferManager;
  debugging::ShaderDebugPrinter& debugPrinter;

  QPointer<scene::CameraComponent> cameraComponent;

  debugging::DebugRenderer::List debugDrawList_Backbuffer;
  debugging::DebugRenderer visualizeCameras;
  debugging::DebugRenderer visualizeSphereAreaLights;
  debugging::DebugRenderer visualizeRectAreaLights;
  debugging::DebugRenderer visualizeVoxelGrids;
  debugging::DebugRenderer visualizeVoxelBoundingSpheres;
  debugging::DebugRenderer visualizeBVH;
  debugging::DebugRenderer visualizeBVH_Grid;
  debugging::DebugRenderer visualizeWorldGrid;
  debugging::DebugRenderer visualizeUniformTest;
  debugging::DebugRenderer visualizeBoundingBoxes;
  debugging::DebugRenderer visualizeSceneBoundingBox;

  debugging::DebugRenderer::List debugDrawList_Framebuffer;
  debugging::DebugRenderer visualizePosteffect_OrangeTest;
  debugging::DebugRenderer visualizePosteffect_Voxel_HighlightUnconveiledNegativeDistances;
  debugging::DebugRenderer visualizePosteffect_Voxel_BoundingBox;
  debugging::DebugRenderer visualizePosteffect_Voxel_Cubic_raymarch;
  debugging::DebugRenderer visualizePosteffect_Distancefield_raymarch;
  debugging::DebugRenderer visualizePosteffect_Distancefield_boundingSpheres_raymarch;

  debugging::PosteffectVisualizationDataBlock debugPosteffect;

  glm::ivec2 videoResolution;

  quint32 costsHeatvisionBlackLevel = 0;
  quint32 costsHeatvisionWhiteLevel = 1024;
  quint16 bvh_debug_depth_begin = 0;
  quint16 bvh_debug_depth_end = 2;

  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  Renderer(const glm::ivec2& videoResolution, scene::Scene* scene, StaticMeshBufferManager* staticMeshBufferManager, debugging::ShaderDebugPrinter* debugPrinter);
  virtual ~Renderer();

  void render();
  void update(float deltaTime);

  GLuint64 sceneUniformAddress() const;

  bool adjustRoughness() const;
  void setAdjustRoughness(bool adjustRoughness);
  bool sdfShadows() const;
  void setSDFShadows(bool sdfShadows);
  bool update_grid_camera() const;
  void set_update_grid_camera(bool update_grid_camera);

protected:
  virtual void prepareFramebuffer() = 0;
  virtual void applyFramebuffer() = 0;

  int appendMaterialShader(QSet<QString> preprocessorBlock, const QSet<Material::Type>& materialTypes, const Pass pass);
  void appendMaterialState(gl::FramebufferObject* framebuffer, const QSet<Material::Type>& materialTypes, const Pass pass, int shader, MaterialState::Flags flags);

private:
  // Shaders
  QMap<QPair<Pass, Material::Type>, MaterialState*> materialShaderMetadata;
  Array<ReloadableShader> materialShaders;
  Array<MaterialState> materialStates;

  // Cascaded Grids
  ComputeStep collectAmbientOcclusionToGrid;
  gl::Texture3D* gridTexture[NUM_GRID_CASCADES*2];
  GLuint64 computeTextureHandles[NUM_GRID_CASCADES*2];
  GLuint64 renderTextureHandles[NUM_GRID_CASCADES*2];
  glm::vec3 grid_camera_pos = glm::vec3(NAN), grid_camera_dir = glm::vec3(NAN);
  bool _update_grid_camera : 1;
  void initCascadedGridTextures();
  void deinitCascadedGridTextures();

  // command lists
  bool _needRecapturing : 1;
  gl::CommandList commandList;

  struct CascadedGridsHeader
  {
    GLuint64 gridTextureRender[NUM_GRID_CASCADES];
    GLuint64 gridTextureCompute[NUM_GRID_CASCADES];
    glm::vec4 snappedGridLocation[NUM_GRID_CASCADES];
    glm::vec4 smoothGridLocation[NUM_GRID_CASCADES];
  };

  // Scene uniform buffer
  struct SceneUniformBlock
  {
    glm::mat4 view_projection_matrix;
    glm::vec3 camera_position;
    float totalTime;
    LightBuffer::LightData lightData;
    VoxelBuffer::VoxelHeader voxelHeader;
    CascadedGridsHeader cascadedGrids;

    // Padding & debugging
    quint32 costsHeatvisionBlackLevel;
    quint32 costsHeatvisionWhiteLevel;
    quint16 bvh_debug_depth_begin;
    quint16 bvh_debug_depth_end;
    padding<quint32, 1> _padding2;
  };
  gl::Buffer sceneUniformBuffer;

  // other uniform buffer
  LightBuffer lightUniformBuffer;
  VoxelBuffer voxelUniformBuffer;
  StaticMeshRenderer staticMeshRenderer;

  // debugging
  bool _adjustRoughness : 1;
  bool _sdfShadows : 1;

  CascadedGridsHeader updateCascadedGrids() const;


  bool needRecapturing() const;
  bool needRerecording() const;
  void captureStates();
  void recordCommandlist();
  void recordLightVisualization(gl::CommandListRecorder& recorder, Material::Type materialType, const MaterialState& materialShader, const glm::ivec2& commonTokenList);

  void updateCameraUniform();
  void fillCameraUniform(const scene::CameraParameter& cameraParameter);

  void allShadersReloaded() final override;

private slots:
  void updateCameraComponent(scene::CameraComponent* cameraComponent);
  void forceNewGridCameraPos();
};






} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_RENDERER_H
