#ifndef GLRT_UUID_H
#define GLRT_UUID_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/uuid.h>

namespace glrt {
namespace scene {
namespace resources {

enum class State
{
  NONE,
  REGISTERED,
  LOADING,
  LOADED
};

class ResourceIndex;
class ResourceLoader;
class MaterialData;
class StaticMeshData;


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_DECLARATIONS_H
