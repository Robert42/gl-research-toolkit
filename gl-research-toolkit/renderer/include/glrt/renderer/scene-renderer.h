#ifndef GLRT_RENDERER_RENDERER_H
#define GLRT_RENDERER_RENDERER_H

#include <glrt/scene/scene.h>
#include <glrt/scene/resources/material.h>
#include <glrt/renderer/debugging/visualization-renderer.h>
#include <glrt/renderer/static-mesh-buffer-manager.h>
#include <glrt/renderer/toolkit/shader-storage-format.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>
#include <glrt/renderer/material-buffer.h>
#include <glrt/renderer/declarations.h>


namespace glrt {
namespace renderer {

class Renderer : public QObject
{
  Q_OBJECT
public:
  class DirectLights;
  class Pass;

  scene::Scene& scene;
  StaticMeshBufferManager& staticMeshBufferManager;

  debugging::VisualizationRenderer visualizeCameras;
  debugging::VisualizationRenderer visualizeSphereAreaLights;
  debugging::VisualizationRenderer visualizeRectAreaLights;

  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  DirectLights& directLights();

  Renderer(scene::Scene* scene, StaticMeshBufferManager* staticMeshBufferManager);
  virtual ~Renderer();

  void render();

protected:
  virtual void renderImplementation() = 0;

private:
  struct CameraUniformBlock
  {
    glm::mat4 view_projection_matrix;
    glm::vec3 camera_position;
    padding<float> _padding;
  };

  gl::Buffer cameraUniformBuffer;

  gl::VertexArrayObject staticMeshVertexArrayObject;

  DirectLights* _directLights = nullptr;

  void updateCameraUniform();

  void debugCameraPositions();
};


class Renderer::DirectLights final
{
public:
  Renderer& renderer;

  DirectLights(Renderer* renderer);
  ~DirectLights();

  void bindShaderStoreageBuffers(int sphereAreaLightBindingIndex, int rectAreaLightBindingIndex);
  void bindShaderStoreageBuffers();

private:
  ShaderStorageFormat<scene::SphereAreaLightComponent> sphereAreaShaderStorageBuffer;
  ShaderStorageFormat<scene::RectAreaLightComponent> rectAreaShaderStorageBuffer;
};


class Renderer::Pass final : public QObject
{
  Q_OBJECT
public:
  const scene::resources::Material::Type type;
  Renderer& renderer;
  ReloadableShader shader;

  Pass(Renderer* renderer, scene::resources::Material::Type type, ReloadableShader&& shader);
  Pass(Renderer* renderer, scene::resources::Material::Type type, const QString& materialName, const QSet<QString>& preprocessorBlock);
  ~Pass();

  Pass(const Pass&) = delete;
  Pass(Pass&&) = delete;
  Pass& operator=(const Pass&) = delete;
  Pass& operator=(Pass&&) = delete;

public:
  void render();

public slots:
  void markDirty();

private:
  typedef glm::mat4 MeshInstanceUniform;

  quint64 _cachedStaticStructureCacheIndex = 0;

  struct MaterialRange
  {
    int begin, end;
  };
  struct MeshRange
  {
    StaticMeshBuffer* mesh;
    int begin, end;
  };

  struct StaticMeshBufferVerification;

  MaterialBuffer materialBuffer;

  QVector<MaterialRange> materialRanges;
  QVector<MeshRange> meshRanges;
  QSharedPointer<gl::Buffer> staticMeshInstance_Uniforms;
  GLint meshInstanceUniformOffset = 0;

  bool isDirty;

  void renderStaticMeshes();
  void updateCache();
  void clearCache();
};



} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_RENDERER_H
