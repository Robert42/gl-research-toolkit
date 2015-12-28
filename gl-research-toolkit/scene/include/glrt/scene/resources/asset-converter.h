#ifndef GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
#define GLRT_SCENE_RESOURCES_ASSETCONVERTER_H

#include "resource-uuid.h"

namespace glrt {
namespace scene {
namespace resources {

void convertStaticMesh(const std::string& meshFile, const std::string& sourceFile);

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
