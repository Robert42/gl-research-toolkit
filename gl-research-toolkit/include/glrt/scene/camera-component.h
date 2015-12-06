#ifndef GLRT_SCENE_CAMERACOMPONENT_H
#define GLRT_SCENE_CAMERACOMPONENT_H

#include "entity.h"
#include "camera-parameter.h"

namespace glrt {
namespace scene {

class CameraComponent final : public VisibleComponent
{
  Q_OBJECT
public:
  CameraParameter cameraParameter;

  CameraComponent(Entity& entity, const CameraParameter& cameraParameter);
  ~CameraComponent();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_CAMERACOMPONENT_H
