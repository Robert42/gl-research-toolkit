#ifndef GLRT_DEBUGCAMERA_H
#define GLRT_DEBUGCAMERA_H

#include <glrt/dependencies.h>

#include <glrt/scene/camera.h>

namespace glrt {

class DebugCamera
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

  bool handleEvents(const SDL_Event& event);

  void update(float deltaTime);

  void operator=(const scene::Camera& cameraParameter);

private:
  glm::mat4 camera_orientation_inverse;
  bool movementMode;
};

} // namespace glrt

#endif // GLRT_DEBUGCAMERA_H
