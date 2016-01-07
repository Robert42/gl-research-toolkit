#ifndef GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
#define GLRT_SCENE_RESOURCES_ASSETCONVERTER_H

#include "declarations.h"
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

  QHash<QString, Uuid<StaticMeshData>> meshUuids;
  QHash<QString, Uuid<MaterialData>> materialUuids;
  QHash<QString, Uuid<LightData>> lightUuids;
  QHash<QString, Uuid<Entity>> nodeUuids;
  QHash<QString, Uuid<CameraParameter>> cameraUuids;

  bool shouldImportMesh(const QString& name) const;
  bool shouldImportCamera(const QString& name) const;
  bool shouldImportNode(const QString& name) const;

  static void registerType();

  SceneGraphImportSettings(AngelScriptInterface* interface);

private:
  static bool shouldImport(const QString& name, const QSet<QString>& patternsToImport);
};

void convertStaticMesh(const std::string& meshFile,
                       const std::string& sourceFile, ResourceIndex*);
void convertSceneGraph(const QString& sceneGraphFilename,
                       const QString& sourceFilename,
                       const Uuid<ResourceIndex>& uuid,
                       const SceneGraphImportSettings& settings);

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
