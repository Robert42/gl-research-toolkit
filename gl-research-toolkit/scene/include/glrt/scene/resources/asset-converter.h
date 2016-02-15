#ifndef GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
#define GLRT_SCENE_RESOURCES_ASSETCONVERTER_H

#include <glrt/scene/declarations.h>
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

  QHash<QString, Uuid<StaticMesh>> meshUuids;
  QHash<QString, Uuid<Material>> materialUuids;
  QHash<QString, Uuid<LightSource>> lightUuids;
  QHash<QString, Uuid<Node>> nodeUuids;
  QHash<QString, Uuid<CameraParameter>> cameraUuids;

  bool shouldImportMesh(const QString& name) const;
  bool shouldImportCamera(const QString& name) const;
  bool shouldImportNode(const QString& name) const;

  static void registerType();

  SceneGraphImportSettings(AngelScriptInterface* interface);

private:
  static bool shouldImport(const QString& name, const QSet<QString>& patternsToImport);
};

struct MeshImportSettings final
{
  bool indexed = false;

  static void registerType();
};

struct TextureImportSettings final
{
  int maxResolution = 2048;
  bool scaleDownToMultipleOfTwo = false;

  static void registerType();
};

void convertStaticMesh(const std::string& meshFile,
                       const std::string& sourceFile,
                       const std::string& groupToImport,
                       ResourceIndex*);
void convertSceneGraph(const QString& sceneGraphFilename,
                       const QString& sourceFilename,
                       const Uuid<ResourceIndex>& uuid,
                       const SceneGraphImportSettings& settings, const QString& groupToImport);

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
