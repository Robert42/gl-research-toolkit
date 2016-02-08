#ifndef GLRT_DEBUGCAMERA_H
#define GLRT_DEBUGCAMERA_H

#include <glrt/dependencies.h>

#include <glrt/scene/camera-parameter.h>
#include <glrt/scene/input-handler.h>

namespace glrt {
namespace scene {

class DebugCamera final : public InputHandler
{
public:
  static glm::ivec2 windowSize;
  glm::mat4 projectionMatrix;
  glm::mat4 viewProjectionMatrix;
  glm::vec3 camera_position;

  float movement_speed;
  float rotation_speed;
  QString loadedName;
  bool locked;

  DebugCamera();

  bool handleEvent(const SDL_Event& event) override;

  void update(float deltaTime);

  void operator=(const scene::CameraParameter& cameraParameter);

private:
  glm::mat4 camera_orientation_inverse;
  bool movementMode;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_DEBUGCAMERA_H
