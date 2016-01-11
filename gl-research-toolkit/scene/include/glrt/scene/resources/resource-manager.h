#ifndef GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H
#define GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H

#include <glrt/scene/resources/static-mesh-loader.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {

class Scene;

namespace resources {


class ResourceManager
{
public:
  typedef ResourceIndex Index;

  StaticMeshLoader& staticMeshLoader;

  ResourceManager(StaticMeshLoader* staticMeshLoader);
  ResourceManager(ResourceManager&&) = delete;
  void operator=(const ResourceManager&) = delete;
  void operator=(ResourceManager&&) = delete;

  virtual ~ResourceManager();

  QString labelForResourceUuid(const QUuid& uuid, const QString& fallback);
  QString labelForResourceUuid(const QUuid& uuid);

  virtual Index* indexForResourceUuid(const QUuid& uuid, Index* fallback=nullptr) = 0;
};


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H
