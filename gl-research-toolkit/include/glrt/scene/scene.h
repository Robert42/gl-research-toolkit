#ifndef GLRT_SCENE_SCENE_H
#define GLRT_SCENE_SCENE_H

#include <glrt/dependencies.h>
#include <glrt/scene/static-mesh.h>

#include <glhelper/shaderobject.hpp>
#include <glhelper/buffer.hpp>
#include <glhelper/texture2d.hpp>

namespace glrt {
namespace scene {

class Scene final : public QObject
{
  Q_OBJECT
public:
  Scene(const Scene&) = delete;
  Scene(Scene&&) = delete;
  Scene& operator=(const Scene&) = delete;
  Scene& operator=(Scene&&) = delete;

  Scene();

  void render();

private:
  struct MaterialInstance
  {
  public:
    gl::Buffer buffer;
    std::vector<StaticMesh> staticMesh;
  };

  struct MaterialMeshList
  {
    gl::ShaderObject shaderObject;
    std::vector<MaterialInstance> materialInstanceUniform;

    MaterialMeshList(gl::ShaderObject&& shaderObject);

    MaterialMeshList() = delete;
    MaterialMeshList(const MaterialMeshList&) = delete;
    MaterialMeshList(MaterialMeshList&&) = delete;
    MaterialMeshList& operator=(const MaterialMeshList&) = delete;
    MaterialMeshList& operator=(MaterialMeshList&&) = delete;

    void render();
  };

  // Materials without blending
  MaterialMeshList plainColorMeshes;
  MaterialMeshList texturedMeshes;
  MaterialMeshList maskedMeshes;

  // Materials with blending
  MaterialMeshList transparentMeshes;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENE_H
