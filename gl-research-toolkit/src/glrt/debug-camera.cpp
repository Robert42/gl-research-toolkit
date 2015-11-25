#include <glrt/debug-camera.h>

namespace glrt {

DebugCamera::DebugCamera()
{
  camera_position = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
  camera_orientation = glm::mat4(1);
  projectionMatrix = glm::perspectiveFov(glm::radians(90.f), 640.f, 480.f, 0.001f, 100.f ); // TODO use the real window size;;

  update();
}

bool DebugCamera::handleEvents(const SDL_Event& event)
{
  const glm::mat4 I = glm::mat4(1);

  switch(event.type)
  {
  case SDL_MOUSEMOTION:
  {
    const glm::vec2 angle = glm::radians(1.f) * glm::vec2(event.motion.xrel, event.motion.yrel);

    camera_orientation = camera_orientation *
                         glm::rotate(I, angle.x, glm::vec3(0, 1, 0)) *
                         glm::rotate(I, angle.y, glm::vec3(1, 0, 0));
    update();
    return true;
  }
  default:
    return false;
  }
}

void DebugCamera::update()
{
  viewMatrix = camera_orientation * camera_position;
}

} // namespace glrt

