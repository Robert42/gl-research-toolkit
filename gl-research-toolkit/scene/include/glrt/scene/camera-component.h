#ifndef GLRT_SCENE_CAMERACOMPONENT_H
#define GLRT_SCENE_CAMERACOMPONENT_H

#include "entity.h"
#include "camera.h"

namespace glrt {
namespace scene {

class CameraComponent final : public Entity::Component
{
public:
  Camera cameraParameter;

  CameraComponent(Entity& entity, const Uuid<CameraComponent>& uuid, const Camera& cameraParameter, bool isMovable=false);
  ~CameraComponent();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_CAMERACOMPONENT_H
