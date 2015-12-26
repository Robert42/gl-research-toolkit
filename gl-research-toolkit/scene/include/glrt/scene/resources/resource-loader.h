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

  void startLoading(const QUuid& uuid);
  void loadNow(const QUuid& uuid);

  void registerStaticMeshFile(const StaticMeshUuid& uuid, const QString& filename);
  virtual void loadStaticMesh(const StaticMeshUuid& uuid, const StaticMeshData& staticMeshData) = 0;
  virtual void waitForStaticMeshToBeLoaded(const StaticMeshUuid& uuid) = 0;

  virtual void loadMaterial(const StaticMeshUuid& uuid, const MaterialData& materialData) = 0;
  virtual void waitForMaterialToBeLoaded(const StaticMeshUuid& uuid) = 0;

private:
  ResourceIndex& index;

  QHash<QUuid, QString> registeredStaticMeshFiles;

  void _loadResource(const QUuid& uuid, bool loadNow);

  StaticMeshData loadStaticMeshFromFile(const QString& filename) const;
};

} // namespace resources
} // namespace scene
} // namespace glrt

#include "resource-uuid-loader.inl"

#endif // GLRT_SCENE_RESOURCE_RESOURCELOADER_H
