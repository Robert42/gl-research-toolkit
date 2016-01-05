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
  QHash<QString, Uuid<StaticMeshData>> meshUuids;
  QHash<QString, Uuid<MaterialData>> materialUuids;
  QHash<QString, Uuid<LightData>> lightUuids;

  void set_meshesToImport(AngelScript::CScriptArray* meshesToImport);
  void set_meshUuids(AngelScript::CScriptDictionary* meshUuids);
  void set_materialUuids(AngelScript::CScriptDictionary* meshUuids);
  void set_lightUuids(AngelScript::CScriptDictionary* lightUuids);

  AngelScript::CScriptArray* get_meshesToImport();
  AngelScript::CScriptDictionary* get_meshUuids();
  AngelScript::CScriptDictionary* get_materialUuids();
  AngelScript::CScriptDictionary* get_lightUuids();

  static void registerType();

private:
  static SceneGraphImportSettings* create();
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
