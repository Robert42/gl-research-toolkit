#ifndef GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
#define GLRT_SCENE_RESOURCES_RESOURCEINDEX_H

#include <glrt/toolkit/uuid.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/resources/light-source.h>
#include <glrt/scene/resources/material.h>
#include <glrt/scene/resources/texture-sampler.h>
#include <glrt/scene/resources/texture.h>
#include <glrt/scene/resources/voxel-data.h>

namespace glrt {
namespace scene {
namespace resources {
namespace uuids {

const Uuid<ResourceIndex> fallbackIndex("{8f26cd17-687c-4aab-946a-079740237011}");
const Uuid<Material> fallbackMaterial("{a8f3fb1b-1168-433b-aaf8-e24632cce156}");
const Uuid<LightSource> fallbackLight("{893463c4-143a-406f-9ef7-3506817d5837}");
const Uuid<Texture> fallbackDiffuseTexture("{8080488b-66e6-4763-aa5e-2393c7865139}");
const Uuid<Texture> fallbackNormalTexture("{d3d320ce-95b0-4bff-ba65-381ad005b012}");
const Uuid<Texture> blackTexture("{a8b7ec89-cf66-4ef6-b68f-ec98d86dc6fb}");
const Uuid<Texture> whiteTexture("{fa46351a-eed3-4b6c-91f6-a8a62a6c6b02}");
const Uuid<Texture> fallbackSRMOTexture("{1a6d4dc1-58b2-47e9-beb6-724da5d2d98f}");
const Uuid<Texture> dfg("{dd867b52-a63d-40c9-8e00-07348d6577bf}");
const Uuid<StaticMesh> unitSphereMesh("{0c8b1ea9-1c5c-4966-98ef-5521fd2bf1ce}");
const Uuid<StaticMesh> unitRectMesh("{eb1697d3-75a9-43dc-9b3e-5daff21b9731}");

} // uuids



class Texture;


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
  void loadAllSubdirectoriesExcept(const std::string& filename);

  void addScriptIncludeDirectory(const std::string& filename);

  void registerStaticMesh(const Uuid<StaticMesh>& uuid, const std::string& mesh_file);
  void registerLightSource(const Uuid<LightSource>& uuid, const LightSource& light);
  void registerMaterial(const Uuid<Material>& uuid, const Material& material);
  void registerSceneLayerFile(const Uuid<SceneLayer>& uuid, const std::string& file);
  void registerSceneFile(const Uuid<Scene>& uuid, const std::string& file);
  void registerTexture_std_string(const Uuid<Texture>& uuid, const std::string& file, const TextureSampler& textureSampler);
  void registerTexture(const Uuid<Texture>& uuid, const QFileInfo& file, const TextureSampler& textureSampler);

  void registerVoxelizedMesh(const Uuid<VoxelIndex>& uuid, const Uuid<StaticMesh>& meshUuid, Voxelizer::FieldType fieldType, const VoxelIndex& voxelIndex);

  bool isRegistered(const QUuid& uuid) const;

  QSet<QUuid> allRegisteredResources;

  QHash<Uuid<Scene>, QString> sceneFiles;
  QHash<Uuid<Texture>, Texture> textures;
  QHash<Uuid<Texture>, TextureSampler> defaultTextureSamplers;
  QHash<Uuid<StaticMesh>, QString> staticMeshAssetsFiles;
  QHash<Uuid<StaticMesh>, QHash<Voxelizer::FieldType, Uuid<VoxelIndex>>> voxelIndicesIndex;
  QHash<Uuid<VoxelIndex>, VoxelIndex> voxelIndices;
  QHash<Uuid<LightSource>, LightSource> lightSources;
  QHash<Uuid<Material>, Material> materials;
  QHash<Uuid<SceneLayer>, QString> sceneLayerFiles;

  QHash<QUuid, QString> labels;

private:
  static ResourceIndex* _fallback;

  QVector<QDir> scriptIncludeDirectories;

  void validateNotYetRegistered(const QUuid& uuid) const;
  void registerFallbackIndex();
};




} // namespace resources
} // namespace glrt
} // namespace scene

#include "resource-index.inl"

#endif // GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
