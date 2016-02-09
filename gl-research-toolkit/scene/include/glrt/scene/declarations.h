#ifndef GLRT_SCENE_DECLARATIONS_H
#define GLRT_SCENE_DECLARATIONS_H

#include <glrt/scene/resources/declarations.h>


namespace glrt {
namespace scene {
namespace implementation {
struct GlobalCoordArrayOrder;
} // namespace implementation

enum class CameraSlot
{
  MAIN_CAMERA
};

class CameraParameter;

class Scene;
class SceneLayer;
class Node;

class LightComponent;
class CameraComponent;
class StaticMeshComponent;

namespace uuids {
const Uuid<SceneLayer> debugCameraLayer("{af7b2fab-9e07-40b9-8a77-591eae9a7d12}");
const Uuid<CameraComponent> debugCameraComponent("{635544d5-207e-4c93-8d01-65f473276f2c}");
} // namespace uuids

} // namespace scene
} // namespace glrt


#endif // GLRT_SCENE_DECLARATIONS_H
