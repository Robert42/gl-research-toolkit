#ifndef GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
#define GLRT_SCENE_RESOURCES_RESOURCEINDEX_H

#include <glrt/toolkit/uuid.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/resources/light-source.h>
#include <glrt/scene/resources/material.h>

namespace glrt {
namespace scene {
namespace resources {
namespace uuids {

const Uuid<ResourceIndex> fallbackIndex("{8f26cd17-687c-4aab-946a-079740237011}");
const Uuid<Material> fallbackMaterial("{a8f3fb1b-1168-433b-aaf8-e24632cce156}");
const Uuid<LightSource> fallbackLight("{893463c4-143a-406f-9ef7-3506817d5837}");

} // uuids


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
  static const ResourceIndex fallback;

  ResourceIndex(const Uuid<ResourceIndex>& uuid);

  const Uuid<ResourceIndex> uuid;

  ResourceIndex(const ResourceIndex&) = delete;
  ResourceIndex(ResourceIndex&&) = delete;
  ResourceIndex& operator=(const ResourceIndex&) = delete;
  ResourceIndex& operator=(ResourceIndex&&) = delete;

  static void registerAngelScriptAPI();

  void loadIndex(const std::string& filename);
  void loadIndexedDirectory(const std::string& filename);

  void registerStaticMesh(const Uuid<StaticMeshData>& uuid, const std::string& mesh_file);
  void registerLightSource(const Uuid<LightSource>& uuid, const LightSource& light);
  void registerMaterial(const Uuid<Material>& uuid, const Material& material);
  void registerSceneLayerFile(const Uuid<SceneLayer>& uuid, const std::string& file);
  void registerSceneFile(const Uuid<Scene>& uuid, const std::string& file);

  bool isRegistered(const QUuid& uuid) const;

  QSet<QUuid> allRegisteredResources;

  QHash<Uuid<Scene>, QString> sceneFiles;
  QHash<Uuid<StaticMeshData>, QString> staticMeshAssetsFiles;
  QHash<Uuid<LightSource>, LightSource> lightSources;
  QHash<Uuid<Material>, Material> materials;
  QHash<Uuid<SceneLayer>, QString> scenelayerFiles;

  QHash<QUuid, QString> labels;

private:
  void validateNotYetRegistered(const QUuid& uuid) const;
  void registerFallbackIndex();
};




} // namespace resources
} // namespace glrt
} // namespace scene

#include "resource-index.inl"

#endif // GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
