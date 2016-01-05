#ifndef GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
#define GLRT_SCENE_RESOURCES_ASSETCONVERTER_H

#include "declarations.h"
#include <angelscript-integration/collection-converter.h>
#include <angelscript-integration/ref-counted-object.h>


namespace glrt {
namespace scene {
namespace resources {

struct SceneGraphImportSettings : public AngelScriptIntegration::RefCountedObject
{
  QSet<QString> meshesToImport = QSet<QString>({".*"});
  QSet<QString> camerasToImport = QSet<QString>({".*"});
  QSet<QString> nodesToImport = QSet<QString>({".*"});
  QHash<QString, Uuid<StaticMeshData>> meshUuids;
  QHash<QString, Uuid<MaterialData>> materialUuids;
  QHash<QString, Uuid<LightData>> lightUuids;
  QHash<QString, Uuid<Entity>> nodeUuids;
  QHash<QString, Uuid<CameraParameter>> cameraUuids;

  void set_meshesToImport(AngelScript::CScriptArray* meshesToImport);
  void set_camerasToImport(AngelScript::CScriptArray* camerasToImport);
  void set_nodesToImport(AngelScript::CScriptArray* nodesToImport);

  void set_meshUuids(AngelScript::CScriptDictionary* meshUuids);
  void set_materialUuids(AngelScript::CScriptDictionary* meshUuids);
  void set_lightUuids(AngelScript::CScriptDictionary* lightUuids);
  void set_nodeUuids(AngelScript::CScriptDictionary* nodeUuids);
  void set_cameraUuids(AngelScript::CScriptDictionary* cameraUuids);

  AngelScript::CScriptArray* get_meshesToImport();
  AngelScript::CScriptArray* get_camerasToImport();
  AngelScript::CScriptArray* get_nodesToImport();

  AngelScript::CScriptDictionary* get_meshUuids();
  AngelScript::CScriptDictionary* get_materialUuids();
  AngelScript::CScriptDictionary* get_lightUuids();
  AngelScript::CScriptDictionary* get_nodeUuids();
  AngelScript::CScriptDictionary* get_cameraUuids();

  bool shouldImportMesh(const QString& name) const;
  bool shouldImportCamera(const QString& name) const;
  bool shouldImportNode(const QString& name) const;

  static void registerType();

private:
  static SceneGraphImportSettings* create();
  static bool shouldImport(const QString& name, const QSet<QString>& patternsToImport);
};

void convertStaticMesh(const std::string& meshFile,
                       const std::string& sourceFile);
void convertSceneGraph(const QString& sceneGraphFilename,
                       const QString& sourceFilename,
                       const SceneGraphImportSettings& settings);

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
