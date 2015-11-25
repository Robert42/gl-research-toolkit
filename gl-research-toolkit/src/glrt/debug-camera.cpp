#include <glrt/debug-camera.h>

namespace glrt {

DebugCamera::DebugCamera(SDL_Window* sdlWindow)
{
  glm::ivec2 size;

  SDL_GetWindowSize(sdlWindow, &size.x, &size.y);

  camera_position = glm::vec3(0, 0, -5);
  camera_orientation = glm::mat4(1);
  projectionMatrix = glm::perspectiveFov<float>(glm::radians(90.f), size.x, size.y, 0.001f, 100.f );

  movementMode = false;

  movement_speed = 5.f;
  rotation_speed = glm::radians(1.f);
}

bool DebugCamera::handleEvents(const SDL_Event& event)
{
  const glm::mat4 I = glm::mat4(1);
  const glm::vec3 x(1, 0, 0);
  const glm::vec3 y(0, 1, 0);
  const glm::vec3 z(0, 0, 1);

  switch(event.type)
  {
  case SDL_MOUSEMOTION:
    if(movementMode)
    {
      const glm::vec2 angle = rotation_speed * glm::vec2(event.motion.xrel, event.motion.yrel);

      camera_orientation = glm::rotate(I, angle.y, x) *
                           camera_orientation *
                           glm::rotate(I, angle.x, y);
      return true;
    }
    return false;
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    switch(event.button.button)
    {
    case SDL_BUTTON_RIGHT:
      this->movementMode = event.button.state == SDL_PRESSED;
      SDL_SetRelativeMouseMode(this->movementMode ? SDL_TRUE : SDL_FALSE);
      return true;
    default:
      return false;
    }
  default:
    return false;
  }
}

void DebugCamera::update(float deltaTime)
{
  const glm::mat4 I = glm::mat4(1);

  if(movementMode)
  {
    const Uint8* state = SDL_GetKeyboardState(nullptr);

    SDL_Scancode w = SDL_GetScancodeFromKey(SDLK_w);
    SDL_Scancode a = SDL_GetScancodeFromKey(SDLK_a);
    SDL_Scancode s = SDL_GetScancodeFromKey(SDLK_s);
    SDL_Scancode d = SDL_GetScancodeFromKey(SDLK_d);
    SDL_Scancode q = SDL_GetScancodeFromKey(SDLK_q);
    SDL_Scancode e = SDL_GetScancodeFromKey(SDLK_e);

    glm::vec3 key_input(state[d]-state[a],
                        state[e]-state[q],
                        state[s]-state[w]);

    key_input = (glm::inverse(camera_orientation) * glm::vec4(-key_input, 0)).xyz();

    camera_position +=  key_input * deltaTime * movement_speed;
  }

  viewMatrix = camera_orientation * glm::translate(I, camera_position);
}

} // namespace glrt

