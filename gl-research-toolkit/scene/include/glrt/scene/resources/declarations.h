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
class MaterialData; // #TODO rename to Material
class StaticMeshData; // #TODO rename to StaticMesh
class LightData; // #TODO rename to Light
class Entity; // #TODO rename to Node?

} // namespace resources

class CameraParameter; // #TODO rename to Camera

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_DECLARATIONS_H
