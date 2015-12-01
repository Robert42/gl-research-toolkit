#ifndef GLRT_SCENE_RENDERER_H
#define GLRT_SCENE_RENDERER_H

#include "scene.h"

namespace glrt {
namespace scene {

class Renderer
{
public:

  class Pass;

  Scene& scene;

  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

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

  void updateSceneUniform();
};


class Renderer::Pass final
{
public:
  Renderer& renderer;
  gl::ShaderObject shaderObject;

  Pass(Renderer* renderer, gl::ShaderObject&& shaderObject);
  Pass(Renderer* renderer, const QString& materialName, const QStringList& preprocessorBlock);

  Pass(const Pass&) = delete;
  Pass(Pass&&) = delete;
  Pass& operator=(const Pass&) = delete;
  Pass& operator=(Pass&&) = delete;

public:
  void render();

private:
  quint64 _cachedStaticStructureCacheIndex = 0;

  struct MaterialInstanceRange
  {
    Material* material;
    int begin, end;
  };
  struct MeshRange
  {
    StaticMesh* material;
    int begin, end;
  };

  std::vector<MaterialInstanceRange> materialInstanceRanges;
  std::vector<MeshRange> meshRanges;
  std::vector<gl::Buffer*> staticMeshInstance_Uniforms;

  void updateCache();
};



} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RENDERER_H
