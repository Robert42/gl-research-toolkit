#ifndef GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
#define GLRT_SCENE_RESOURCES_RESOURCEINDEX_H

#include <glrt/toolkit/uuid.h>
#include <glrt/scene/declarations.h>

namespace glrt {
namespace scene {
namespace resources {


struct StaticMeshImportSettings
{
  std::string sourceFile;
  std::string targetFile;
  std::string object_group_to_import;
  bool apply_transformation;
};

class ResourceIndex final
{
public:
  ResourceIndex(const Uuid<ResourceIndex>& uuid);

  const Uuid<ResourceIndex> uuid;

  ResourceIndex(const ResourceIndex&) = delete;
  ResourceIndex(ResourceIndex&&) = delete;
  ResourceIndex& operator=(const ResourceIndex&) = delete;
  ResourceIndex& operator=(ResourceIndex&&) = delete;

  static void registerAngelScriptAPI();

  void loadIndex(const std::string& filename);
  void loadIndexedDirectory(const std::string& filename);

  State stateOf(const QUuid& uuid) const;
  bool isRegistered(const QUuid& uuid) const;
  bool isLoading(const QUuid& uuid) const;
  bool isLoaded(const QUuid& uuid) const;

  void registerStaticMesh(const Uuid<StaticMeshData>& uuid, const std::string& mesh_file);
  void registerLightSource(const Uuid<LightSource>& uuid, const LightSource& light);
  void registerMaterial(const Uuid<Material>& uuid, const Material& material);

private:
  friend class ResourceLoader;

  QSet<QUuid> unloadedRessources;
  QSet<QUuid> loadingRessources;
  QSet<QUuid> loadedRessources;

  QHash<Uuid<StaticMeshData>, QString> staticMeshAssetsFiles;
  QHash<Uuid<LightSource>, LightSource> lightSources;
  QHash<Uuid<Material>, Material> materials;

  void _loadResource(ResourceLoader* loader, const QUuid& uuid, bool loadNow);
  void waitForAssetToBeLoaded(const QUuid& uuid);

  bool classInvariant();
};


} // namespace resources
} // namespace glrt
} // namespace scene

#include "resource-index.inl"

#endif // GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
