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

  debugging::DebugRenderer::List debugDrawList_Lines;
  debugging::DebugRenderer visualizeCameras;
  debugging::DebugRenderer visualizeSphereAreaLights;
  debugging::DebugRenderer visualizeRectAreaLights;
  debugging::DebugRenderer visualizeVoxelGrids;
  debugging::DebugRenderer visualizeVoxelBoundingSpheres;
  debugging::DebugRenderer visualizeSdfCandidateGrid;
  debugging::DebugRenderer visualizeSdfCandidateCell;
  debugging::DebugRenderer visualizeSdfFallbackGrid;
  debugging::DebugRenderer visualizeBVH;
  debugging::DebugRenderer visualizeBVH_Grid;
  debugging::DebugRenderer visualizeWorldGrid;
  debugging::DebugRenderer visualizeUniformTest;
  debugging::DebugRenderer visualizeBoundingBoxes;
  debugging::DebugRenderer visualizeSceneBoundingBox;

  debugging::DebugRenderer::List debugDrawList_Posteffects;
  debugging::DebugRenderer visualizePosteffect_OrangeTest;
  debugging::DebugRenderer visualizePosteffect_Voxel_HighlightUnconveiledNegativeDistances;
  debugging::DebugRenderer visualizePosteffect_Voxel_BoundingBox;
  debugging::DebugRenderer visualizePosteffect_Voxel_Cubic_raymarch;
  debugging::DebugRenderer visualizePosteffect_Distancefield_raymarch;
  debugging::DebugRenderer visualizePosteffect_Fallback_Distancefield_raymarch;
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

  Renderer(const glm::ivec2& videoResolution, scene::Scene* scene, StaticMeshBufferManager* staticMeshBufferManager, debugging::ShaderDebugPrinter* debugPrinter, const QSet<QString>& debug_posteffect_preprocessor);
  virtual ~Renderer();

  void render();
  void update(float deltaTime);

  GLuint64 sceneUniformAddress() const;

  bool adjustRoughness() const;
  void setAdjustRoughness(bool adjustRoughness);
  bool sdfShadows() const;
  void setSDFShadows(bool sdfShadows, bool autoReloadShader=true);
  bool ambientOcclusionSDF() const;
  void setAmbientOcclusionSDF(bool sdfAO, bool autoReloadShader=true);
  bool ambientOcclusionTexture() const;
  void setAmbientOcclusionTexture(bool textureAO);
  bool ibl_Diffuse() const;
  void setIBL_Diffuse(bool ibl);
  bool ibl_Specular() const;
  void setIBL_Specular(bool ibl);
  bool update_grid_camera() const;
  void set_update_grid_camera(bool update_grid_camera);

  QImage takeScreenshot();

  virtual bool isForward() const = 0;

protected:
  virtual void prepareFramebuffer() = 0;
  virtual void applyFramebuffer() = 0;

  int appendMaterialShader(QSet<QString> preprocessorBlock, const QSet<Material::Type>& materialTypes, const Pass pass);
  void appendMaterialState(gl::FramebufferObject* framebuffer, const QSet<Material::Type>& materialTypes, const Pass pass, int shader, MaterialState::Flags flags);

  void render_debugPosteffects();
  void render_debugLines();

private:
  typedef scene::resources::utilities::GlTexture GlTexture;

  // Shaders
  QMap<QPair<Pass, Material::Type>, MaterialState*> materialShaderMetadata;
  Array<ReloadableShader> materialShaders;
  Array<MaterialState> materialStates;

  // Cascaded Grids
  ComputeStep collectAmbientOcclusionToGrid1, collectAmbientOcclusionToGrid2, collectAmbientOcclusionToGrid3;
  ComputeStep* collectAmbientOcclusionToGrid[4];
  GlTexture gridTexture[MAX_NUM_GRID_CASCADES*2];
  GLuint64 computeTextureHandles[MAX_NUM_GRID_CASCADES*2];
  GLuint64 renderTextureHandles[MAX_NUM_GRID_CASCADES*2];

  GlTexture gridOcclusionTexture[MAX_NUM_GRID_CASCADES];
  GLuint64 computeOcclusionTextureHandles[MAX_NUM_GRID_CASCADES];
  GLuint64 renderOcclusionTextureHandles[MAX_NUM_GRID_CASCADES];

  glm::vec3 grid_camera_pos = glm::vec3(NAN), grid_camera_dir = glm::vec3(NAN);
  bool _update_grid_camera : 1;
  void initCascadedGridTextures();
  void deinitCascadedGridTextures();

  // command lists
  bool _needRecapturing : 1;
  gl::CommandList commandList;

  struct CascadedGridsHeader
  {
    GLuint64 gridTexture[MAX_NUM_GRID_CASCADES];
//#if BVH_USE_GRID_OCCLUSION
    GLuint64 occlusionTexture[MAX_NUM_GRID_CASCADES];
//#else
//    padding<GLuint64, NUM_GRID_CASCADES%2> _padding;
//#endif
    glm::vec4 snappedGridLocation[MAX_NUM_GRID_CASCADES];
    glm::vec4 smoothGridLocation[MAX_NUM_GRID_CASCADES];
  };
  struct AoCollectHeader
  {
    AlignedImageHandle gridTexture[MAX_NUM_GRID_CASCADES];
    AlignedImageHandle occlusionTexture[MAX_NUM_GRID_CASCADES];
  };

  // Scene uniform buffer
  struct SceneUniformBlock
  {
    glm::mat4 view_projection_matrix;
    glm::vec3 camera_position;
    float totalTime;
    LightBuffer::LightData lightData;
    VoxelBuffer::VoxelHeader voxelHeader;
    VoxelBuffer::CandidateGridHeader candidateGrid;
    CascadedGridsHeader cascadedGrids;
    quint64 sky_texture;
    quint64 dfg_lut_texture;

    // Padding & debugging
    quint32 costsHeatvisionBlackLevel;
    quint32 costsHeatvisionWhiteLevel;
    quint16 bvh_debug_depth_begin;
    quint16 bvh_debug_depth_end;
    padding<quint32, 1> _padding2;
  };
  gl::Buffer sceneUniformBuffer;
  gl::Buffer aoCollectHeaderUniformBuffer;

  struct SkyTexture
  {
    GLuint64 equirectengular_view;
    GLuint64 ibl_ggx;
    GLuint64 ibl_diffuse;
    GLuint64 ibl_cone_45;
    GLuint64 ibl_cone_60;
    GLuint64 dfg;
  }skyTexture;

  // other uniform buffer
  LightBuffer lightUniformBuffer;
  VoxelBuffer voxelUniformBuffer;
  StaticMeshRenderer staticMeshRenderer;


  gl::Buffer screenspace_quad_buffer;
  void init_screenspace_quad_buffer();

  // debugging
  bool _adjustRoughness : 1;
  bool _sdfShadows : 1;
  bool _sdfAO : 1;
  bool _textureAO : 1;
  bool _iblDiffuse : 1;
  bool _iblSpecular : 1;

  CascadedGridsHeader updateCascadedGrids() const;

  bool needRecapturing() const;
  bool needRerecording() const;
  void captureStates();
  void recordCommandlist();
  void recordLightVisualization(gl::CommandListRecorder& recorder, Material::Type materialType, const MaterialState& materialShader, const glm::ivec2& commonTokenList);
  void recordSky(gl::CommandListRecorder& recorder, Material::Type materialType, const MaterialState& materialShader, const glm::ivec2& commonTokenList);

  void updateCameraUniform();
  void fillCameraUniform(const scene::CameraParameter& cameraParameter);

  void allShadersReloaded() final override;

  void updateBvhLeafGrid();
  void update_aoCollectHeader();

private slots:
  void updateCameraComponent(scene::CameraComponent* cameraComponent);
  void forceNewGridCameraPos();
  void updateSky();
  void updateStatistics();
};






} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_RENDERER_H
