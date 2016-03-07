#ifndef GLRT_RENDERER_RENDERER_H
#define GLRT_RENDERER_RENDERER_H


#include <glrt/scene/scene.h>
#include <glrt/scene/camera-component.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/resources/material.h>
#include <glrt/renderer/declarations.h>
#include <glrt/renderer/debugging/visualization-renderer.h>
#include <glrt/renderer/static-mesh-buffer-manager.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>
#include <glrt/renderer/static-mesh-renderer.h>
#include <glrt/renderer/declarations.h>
#include <glrt/renderer/light-buffer.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/renderer/material-shader.h>

#include <glhelper/framebufferobject.hpp>
#include <glhelper/texture2d.hpp>


namespace glrt {
namespace renderer {

#define GLRT_ENABLE_SCENE_RENDERING 0

class Renderer : public QObject
{
  Q_OBJECT
public:
  scene::Scene& scene;
  StaticMeshBufferManager& staticMeshBufferManager;

  QPointer<scene::CameraComponent> cameraComponent;

  debugging::VisualizationRenderer visualizeCameras;
  debugging::VisualizationRenderer visualizeSphereAreaLights;
  debugging::VisualizationRenderer visualizeRectAreaLights;

  glm::ivec2 videoResolution;

  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  Renderer(const glm::ivec2& videoResolution, scene::Scene* scene, StaticMeshBufferManager* staticMeshBufferManager);
  virtual ~Renderer();

  void render();

protected:
  virtual void clearFramebuffer() = 0;
  virtual void callExtraCommandLists(){}
  virtual void applyFramebuffer() = 0;

  void appendMaterialShader(gl::FramebufferObject* framebuffer, QSet<QString> preprocessorBlock, const QSet<Material::Type>& materialTypes, const Pass pass);

  GLuint64 sceneVertexUniformAddress() const;
  GLuint64 sceneFragmentUniformAddress() const;

private:
  struct SceneVertexUniformBlock
  {
    glm::mat4 view_projection_matrix;
  };

  struct SceneFragmentUniformBlock
  {
    glm::vec3 camera_position;
    padding<float> _padding;
  };

  LightBuffer lightUniformBuffer;
  StaticMeshRenderer<> staticMeshRenderer;
  QMap<QPair<Pass, Material::Type>, MaterialShader*> materialShaderMetadata;
  Array<MaterialShader> materialShaders;

  gl::Texture2D workaroundFramebufferTexture;
  gl::FramebufferObject workaroundFramebuffer;
  gl::Buffer sceneVertexUniformBuffer;
  gl::Buffer sceneFragmentUniformBuffer;
  gl::CommandList commandList;

  bool needRerecording() const;
  void recordCommandlist();

  void updateCameraUniform();
  void fillCameraUniform(const scene::CameraParameter& cameraParameter);

private slots:
  void updateCameraComponent(scene::CameraComponent* cameraComponent);
};






} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_RENDERER_H
