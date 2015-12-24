#ifndef GLRT_SCENE_RENDERER_H
#define GLRT_SCENE_RENDERER_H

#include <glrt/scene/scene.h>
#include <glrt/debugging/visualization-renderer.h>
#include <glrt/toolkit/shader-storage-format.h>
#include <glrt/toolkit/reloadable-shader.h>


namespace glrt {
namespace scene {

class Renderer : public QObject
{
  Q_OBJECT
public:

  class DirectLights;
  class Pass;

  Scene& scene;
  debugging::VisualizationRenderer visualizeCameras;
  debugging::VisualizationRenderer visualizeSphereAreaLights;
  debugging::VisualizationRenderer visualizeRectAreaLights;

  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  DirectLights& directLights();

  Renderer(Scene* scene);
  virtual ~Renderer();

  void render();

protected:
  virtual void renderImplementation() = 0;

private:
  struct SceneUniformBlock
  {
    glm::mat4 view_projection_matrix;
    glm::vec3 camera_position;
    padding<float> _padding;
  };

  gl::Buffer sceneUniformBuffer;

  gl::VertexArrayObject staticMeshVertexArrayObject;

  DirectLights* _directLights = nullptr;

  void updateSceneUniform();


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
  ShaderStorageFormat<SphereAreaLightComponent> sphereAreaShaderStorageBuffer;
  ShaderStorageFormat<RectAreaLightComponent> rectAreaShaderStorageBuffer;
};


class Renderer::Pass final
{
public:
  const MaterialInstance::Type type;
  Renderer& renderer;
  ReloadableShader shader;

  Pass(Renderer* renderer, MaterialInstance::Type type, ReloadableShader&& shader);
  Pass(Renderer* renderer, MaterialInstance::Type type, const QString& materialName, const QSet<QString>& preprocessorBlock);
  ~Pass();

  Pass(const Pass&) = delete;
  Pass(Pass&&) = delete;
  Pass& operator=(const Pass&) = delete;
  Pass& operator=(Pass&&) = delete;

public:
  void render();

private:
  typedef glm::mat4 MeshInstanceUniform;

  quint64 _cachedStaticStructureCacheIndex = 0;

  struct MaterialInstanceRange
  {
    MaterialInstance* materialInstance;
    int begin, end;
  };
  struct MeshRange
  {
    StaticMesh* mesh;
    int begin, end;
  };

  std::vector<MaterialInstanceRange> materialInstanceRanges;
  std::vector<MeshRange> meshRanges;
  QSharedPointer<gl::Buffer> staticMeshInstance_Uniforms;
  GLint meshInstanceUniformOffset = 0;

  void renderStaticMeshes();
  void updateCache();
  void clearCache();
};



} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RENDERER_H
