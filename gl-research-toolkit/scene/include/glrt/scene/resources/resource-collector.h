#ifndef GLRT_SCENE_RESOURCES_RESOURCECOLLECTOR_H
#define GLRT_SCENE_RESOURCES_RESOURCECOLLECTOR_H

#include "resource-manager.h"

namespace glrt {
namespace scene {
namespace resources {

QList<Uuid<Scene>> allRegisteredScenes(const ResourceManager& resourceManager);


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_RESOURCECOLLECTOR_H
