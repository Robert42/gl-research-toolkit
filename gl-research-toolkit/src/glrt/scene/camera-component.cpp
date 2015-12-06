#include <glrt/scene/camera-component.h>

namespace glrt {
namespace scene {


CameraComponent::CameraComponent(Entity& entity, const CameraParameter& cameraParameter)
  : VisibleComponent(entity, false),
    cameraParameter(cameraParameter)
{
}


CameraComponent::~CameraComponent()
{
}


} // namespace scene
} // namespace glrt

