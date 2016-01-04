#ifndef GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
#define GLRT_SCENE_RESOURCES_ASSETCONVERTER_H

#include "declarations.h"


namespace glrt {
namespace scene {
namespace resources {

struct SceneGraphImportSettings
{
  QSet<QString> meshesToImport;
  QHash<QString, QUuid> meshUuids;
  QHash<QString, QUuid> materialUuids;
  QHash<QString, QUuid> lightUuids;
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
