#include <glrt/debug-camera.h>

namespace glrt {

DebugCamera::DebugCamera(SDL_Window* sdlWindow)
{
  glm::ivec2 size;

  SDL_GetWindowSize(sdlWindow, &size.x, &size.y);

  camera_position = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
  camera_orientation = glm::mat4(1);
  projectionMatrix = glm::perspectiveFov<float>(glm::radians(90.f), size.x, size.y, 0.001f, 100.f );

  rotationMode = false;

  update();
}

bool DebugCamera::handleEvents(const SDL_Event& event)
{
  const glm::mat4 I = glm::mat4(1);
  const glm::vec3 x(1, 0, 0);
  const glm::vec3 y(0, 1, 0);

  switch(event.type)
  {
  case SDL_MOUSEMOTION:
    if(rotationMode)
    {
      const glm::vec2 angle = glm::radians(1.f) * glm::vec2(event.motion.xrel, event.motion.yrel);

      camera_orientation = glm::rotate(I, angle.y, x) *
                           camera_orientation *
                           glm::rotate(I, angle.x, y);
      update();
      return true;
    }
    return false;
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    switch(event.button.button)
    {
    case SDL_BUTTON_RIGHT:
      this->rotationMode = event.button.state == SDL_PRESSED;
      SDL_SetRelativeMouseMode(this->rotationMode ? SDL_TRUE : SDL_FALSE);
      return true;
    default:
      return false;
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

