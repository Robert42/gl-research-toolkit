#ifndef GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
#define GLRT_SCENE_RESOURCES_ASSETCONVERTER_H

#include "resource-uuid.h"

namespace glrt {
namespace scene {
namespace resources {

void convertStaticMesh(const std::string& meshFile, const std::string& sourceFile);
void convertSceneGraph(const std::string& sceneGraphFilename, const std::string& sourceFilename);

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
