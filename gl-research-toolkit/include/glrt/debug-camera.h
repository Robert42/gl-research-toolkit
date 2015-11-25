#ifndef GLRT_DEBUGCAMERA_H
#define GLRT_DEBUGCAMERA_H

#include "dependencies.h"

namespace glrt {

class DebugCamera
{
public:
  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;

  DebugCamera(SDL_Window* sdlWindow);

  bool handleEvents(const SDL_Event& event);

private:
  glm::mat4 camera_position;
  glm::mat4 camera_orientation;
  bool rotationMode;

  void update();
};

} // namespace glrt

#endif // GLRT_DEBUGCAMERA_H
