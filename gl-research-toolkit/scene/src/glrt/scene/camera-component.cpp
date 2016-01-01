#include <glrt/scene/camera-component.h>

namespace glrt {
namespace scene {


CameraComponent::CameraComponent(const Uuid<CameraComponent>& uuid, const CameraParameter& cameraParameter, bool isMovable)
  : SpatialComponent(uuid, isMovable),
    cameraParameter(cameraParameter)
{
}


CameraComponent::~CameraComponent()
{
}


} // namespace scene
} // namespace glrt

