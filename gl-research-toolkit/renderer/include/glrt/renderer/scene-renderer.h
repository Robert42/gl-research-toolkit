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
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/renderer/material-state.h>
#include <glrt/renderer/debugging/shader-debug-printer.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>

#include <glhelper/framebufferobject.hpp>
#include <glhelper/texture2d.hpp>


namespace glrt {
namespace renderer {


class Renderer : public QObject, public ReloadableShader::Listener
{
  Q_OBJECT
public:
  scene::Scene& scene;
  StaticMeshBufferManager& staticMeshBufferManager;
  debugging::ShaderDebugPrinter& debugPrinter;

  QPointer<scene::CameraComponent> cameraComponent;

  debugging::DebugRenderer visualizeCameras;
  debugging::DebugRenderer visualizeSphereAreaLights;
  debugging::DebugRenderer visualizeRectAreaLights;

  debugging::DebugRenderer visualizePosteffect_OrangeTest;
  debugging::DebugRenderer::List debugDrawList_Framebuffer;
  debugging::DebugRenderer::List debugDrawList_Backbuffer;

  glrt::scene::ComponentDecorator<glrt::scene::LightComponent> lightComponentVisualization;

  glm::ivec2 videoResolution;

  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  Renderer(const glm::ivec2& videoResolution, scene::Scene* scene, StaticMeshBufferManager* staticMeshBufferManager, debugging::ShaderDebugPrinter* debugPrinter);
  virtual ~Renderer();

  void render();

  GLuint64 sceneVertexUniformAddress() const;
  GLuint64 sceneFragmentUniformAddress() const;

protected:
  virtual void prepareFramebuffer() = 0;
  virtual void applyFramebuffer() = 0;

  int appendMaterialShader(QSet<QString> preprocessorBlock, const QSet<Material::Type>& materialTypes, const Pass pass);
  void appendMaterialState(gl::FramebufferObject* framebuffer, const QSet<Material::Type>& materialTypes, const Pass pass, int shader, MaterialState::Flags flags);

private:
  struct SceneVertexUniformBlock
  {
    glm::mat4 view_projection_matrix;
  };

  struct SceneFragmentUniformBlock
  {
    glm::vec3 camera_position;
    padding<float> _padding;
    LightBuffer::LightData lightData;
  };

  LightBuffer lightUniformBuffer;
  StaticMeshRenderer<> staticMeshRenderer;
  QMap<QPair<Pass, Material::Type>, MaterialState*> materialShaderMetadata;
  Array<ReloadableShader> materialShaders;
  Array<MaterialState> materialStates;

  gl::Buffer sceneVertexUniformBuffer;
  gl::Buffer sceneFragmentUniformBuffer;
  gl::CommandList commandList;

  bool _needRecapturing : 1;

  bool needRecapturing() const;
  bool needRerecording() const;
  void captureStates();
  void recordCommandlist();

  void updateCameraUniform();
  void fillCameraUniform(const scene::CameraParameter& cameraParameter);

  void allShadersReloaded() final override;

private slots:
  void updateCameraComponent(scene::CameraComponent* cameraComponent);
};






} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_RENDERER_H
