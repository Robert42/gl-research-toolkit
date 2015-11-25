#ifndef GLRT_DEBUGCAMERA_H
#define GLRT_DEBUGCAMERA_H

#include "dependencies.h"

namespace glrt {

class DebugCamera
{
public:
  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;
  float movement_speed;
  float rotation_speed;

  DebugCamera(SDL_Window* sdlWindow);

  bool handleEvents(const SDL_Event& event);

  void update(float deltaTime);

private:
  glm::vec3 camera_position;
  glm::mat4 camera_orientation;
  bool movementMode;
};

} // namespace glrt

#endif // GLRT_DEBUGCAMERA_H
