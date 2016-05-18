#ifndef GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
#define GLRT_SCENE_RESOURCES_ASSETCONVERTER_H

#include <glrt/scene/declarations.h>
#include <glrt/scene/resources/texture-file.h>
#include <angelscript-integration/collection-converter.h>
#include <angelscript-integration/ref-counted-object.h>


namespace glrt {
namespace scene {
namespace resources {

struct SceneGraphImportSettings final
{
  class AngelScriptInterface;

  QSet<QString> meshesToImport;
  QSet<QString> camerasToImport;
  QSet<QString> nodesToImport;
  QSet<QString> meshesToVoxelize;
  QSet<QString> meshesToVoxelizeTwoSided;
  QSet<QString> meshesToVoxelizeWithManifold;
  QList<QSet<QString>> meshesToMergeWhenVoxelizing;
  QList<QSet<QString>> meshesToMergeWhenVoxelizingInstanced;

  QHash<QString, float> meshVoxelizeScaleFactors;
  QHash<QString, Uuid<StaticMesh>> meshUuids;
  QHash<QString, Uuid<Material>> materialUuids;
  QHash<QString, Uuid<LightSource>> lightUuids;
  QHash<QString, Uuid<Node>> nodeUuids;
  QHash<QString, Uuid<CameraParameter>> cameraUuids;

  bool shouldImportMesh(const QString& name) const;
  bool shouldImportCamera(const QString& name) const;
  bool shouldImportNode(const QString& name) const;
  float meshVoxelizeScaleFactorForMeshName(const QString& name) const;

  static void registerType();

  SceneGraphImportSettings(AngelScriptInterface* interface);
};

struct MeshImportSettings final
{
  bool indexed = true;

  static void registerType();
};

void convertStaticMesh(const QString& meshFile,
                       const QString& sourceFile,
                       const QString& groupToImport,
                       const MeshImportSettings& meshImportSettings);
void convertTexture(const QString& textureFilename,
                    const QString& sourceFilename,
                    const TextureFile::ImportSettings& textureImportSettings);
void convertSceneGraph(const QString& sceneGraphFilename,
                       const QString& sourceFilename,
                       const Uuid<ResourceIndex>& uuid,
                       const SceneGraphImportSettings& settings,
                       const QString& groupToImport);

bool shouldConvert(const QFileInfo& targetFile, const QFileInfo& sourceFile, const QSet<QString>& converterSourceFile);

extern bool forceReimport_Assets;

#define SHOULD_CONVERT(targetFile, sourceFile) ::glrt::scene::resources::shouldConvert(targetFile, sourceFile, {QString(__FILE__)})

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
