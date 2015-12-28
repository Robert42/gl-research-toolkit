#ifndef GLRT_SCENE_RESOURCE_RESOURCELOADER_H
#define GLRT_SCENE_RESOURCE_RESOURCELOADER_H

#include "resource-uuid.h"
#include "resource-index.h"

#include "static-mesh-data.h"
#include "material-data.h"

namespace glrt {
namespace scene {
namespace resources {

class ResourceLoader final
{
public:
  // The given index instance must exist longer than the new ResourceLoader instance
  ResourceLoader(ResourceIndex* index);
  virtual ~ResourceLoader();

  ResourceLoader(const ResourceLoader&) = delete;
  ResourceLoader(ResourceLoader&&) = delete;
  ResourceLoader& operator=(const ResourceLoader&) = delete;
  ResourceLoader& operator=(ResourceLoader&&) = delete;

  void startLoadingFromFile(const QUuid& uuid);
  void loadNowFromFile(const QUuid& uuid);

  virtual void loadStaticMesh(const StaticMeshUuid& uuid, const StaticMeshData& staticMeshData) = 0;

  void loadResourceFromFile(const QUuid& uuid, bool loadNow);

private:
  ResourceIndex& index;

};

} // namespace resources
} // namespace scene
} // namespace glrt

#include "resource-uuid-loader.inl"

#endif // GLRT_SCENE_RESOURCE_RESOURCELOADER_H
