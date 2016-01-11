#ifndef GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H
#define GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H

#include <glrt/scene/resources/static-mesh-loader.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {

namespace resources {


class ResourceManager
{
public:
  StaticMeshLoader& staticMeshLoader;
  Scene scene;

  ResourceManager(StaticMeshLoader* staticMeshLoader);
  ResourceManager(ResourceManager&&) = delete;
  void operator=(const ResourceManager&) = delete;
  void operator=(ResourceManager&&) = delete;

  virtual ~ResourceManager();

  void loadScene(const Uuid<Scene>& sceneUuid);
};


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H
