#include <glrt/scene/camera-component.h>

namespace glrt {
namespace scene {


CameraComponent::CameraComponent(Entity& entity, const Uuid<CameraComponent>& uuid, const Camera& cameraParameter, bool isMovable)
  : Component(entity, uuid, isMovable),
    cameraParameter(cameraParameter)
{
}


CameraComponent::~CameraComponent()
{
}


} // namespace scene
} // namespace glrt

