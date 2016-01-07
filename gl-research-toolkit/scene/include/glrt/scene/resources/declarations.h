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
class LightSource;
class MaterialData; // #TODO rename to Material
class StaticMeshData; // #TODO rename to StaticMesh

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_DECLARATIONS_H
