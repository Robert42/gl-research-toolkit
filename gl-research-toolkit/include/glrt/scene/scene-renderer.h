#ifndef GLRT_SCENE_RENDERER_H
#define GLRT_SCENE_RENDERER_H

#include <glrt/scene/scene.h>
#include <glrt/debugging/visualization-renderer.h>


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

  DirectLights(const DirectLights&) = delete;
  DirectLights(DirectLights&&) = delete;
  DirectLights& operator=(const DirectLights&) = delete;
  DirectLights& operator=(DirectLights&&) = delete;

  void init();

  void bindShaderStoreageBuffers(int sphereAreaLightBindingIndex, int rectAreaLightBindingIndex);
  void bindShaderStoreageBuffers();

private:
  gl::Buffer* sphereAreaLightShaderStorageBuffer = nullptr;
  gl::Buffer* rectAreaLightShaderStorageBuffer = nullptr;

  void deinit();
};


class Renderer::Pass final
{
public:
  const MaterialInstance::Type type;
  Renderer& renderer;
  gl::ShaderObject shaderObject;

  Pass(Renderer* renderer, MaterialInstance::Type type, gl::ShaderObject&& shaderObject);
  Pass(Renderer* renderer, MaterialInstance::Type type, const QString& materialName, const QStringList& preprocessorBlock);
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
