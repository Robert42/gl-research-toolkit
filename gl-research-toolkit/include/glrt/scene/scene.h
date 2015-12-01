#ifndef GLRT_SCENE_SCENE_H
#define GLRT_SCENE_SCENE_H

#include <glrt/dependencies.h>
#include <glrt/scene/static-mesh.h>
#include <glrt/scene/material.h>
#include <glrt/debug-camera.h>

#include <glhelper/shaderobject.hpp>
#include <glhelper/buffer.hpp>
#include <glhelper/texture2d.hpp>

namespace glrt {
namespace scene {


class Entity;
class StaticMeshComponent;

class Scene final : public QObject
{
  Q_OBJECT
public:
  struct SceneUniformBlock
  {
    glm::mat4 view_projection_matrix;
  };

  DebugCamera camera;

  Scene(const Scene&) = delete;
  Scene(Scene&&) = delete;
  Scene& operator=(const Scene&) = delete;
  Scene& operator=(Scene&&) = delete;

  Scene(SDL_Window* sdlWindow);
  ~Scene();

  bool handleEvents(const SDL_Event& event);
  void update(float deltaTime);

  void render();

private:
  friend class Renderer;
  quint64 cachedStaticStructureCacheIndex;

  friend class Entity;
  friend class StaticMeshComponent;


  struct MaterialPass
  {
    struct MaterialInstance
    {
      struct MeshGroup
      {
        QMap<StaticMeshComponent*, gl::Buffer*> staticMeshComponents;

        void AddStaticMesh(StaticMeshComponent* staticMeshComponent);
        void RemoveStaticMesh(StaticMeshComponent* staticMeshComponent);

        bool isEmpty() const;

        void render(StaticMesh& staticMesh);
      };

      QMap<StaticMesh*, MeshGroup> staticMeshes;

      void AddStaticMesh(StaticMeshComponent* staticMeshComponent);
      void RemoveStaticMesh(StaticMeshComponent* staticMeshComponent);

      bool isEmpty() const;

      void render();
    };

    gl::ShaderObject shaderObject;
    QMap<Material*, MaterialInstance> materialInstanceMeshList;

    MaterialPass(gl::ShaderObject&& shaderObject);

    MaterialPass() = delete;
    MaterialPass(const MaterialPass&) = delete;
    MaterialPass(MaterialPass&&) = delete;
    MaterialPass& operator=(const MaterialPass&) = delete;
    MaterialPass& operator=(MaterialPass&&) = delete;

    void AddStaticMesh(StaticMeshComponent* staticMeshComponent);
    void RemoveStaticMesh(StaticMeshComponent* staticMeshComponent);
    void deinit();

    bool isEmpty() const;

    void render();
  };

  // Materials without blending
  MaterialPass plainColorMeshes;
  MaterialPass texturedMeshes;
  MaterialPass maskedMeshes;

  // Materials with blending
  MaterialPass transparentMeshes;

  gl::VertexArrayObject staticMeshVertexArrayObject;

  QSet<Entity*> _entities;

  gl::Buffer sceneUniformBuffer;

  void AddEntity(Entity* entity);
  void RemoveEntity(Entity* entity);

  void AddStaticMesh(StaticMeshComponent* staticMeshComponent);
  void RemoveStaticMesh(StaticMeshComponent* staticMeshComponent);

  // May return nullptr. The pointer will stay valid until the scene is changed
  MaterialPass* pickMaterialPass(const Material* material);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENE_H
