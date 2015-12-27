#include <glrt/scene/debug-camera.h>
#include <glrt/toolkit/geometry.h>
#include <glrt/toolkit/json.h>

namespace glrt {

glm::ivec2 DebugCamera::windowSize;

DebugCamera::DebugCamera()
{
  *this = scene::CameraParameter::defaultDebugCamera();

  movementMode = false;

  movement_speed = 5.f;
  rotation_speed = glm::radians(1.f);
  locked = false;
}

bool DebugCamera::handleEvents(const SDL_Event& event)
{
  if(locked)
  {
    movementMode = false;
    return false;
  }

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

      camera_orientation_inverse = glm::rotate(I, angle.y, x) *
                                   camera_orientation_inverse *
                                   glm::rotate(I, angle.x, z);
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
  case SDL_WINDOWEVENT:
    switch(event.window.event)
    {
    case SDL_WINDOWEVENT_RESIZED:
      this->windowSize.x = event.window.data1;
      this->windowSize.y = event.window.data2;
      break;
    default:
      break;
    }
    return false;
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

    key_input = transform_direction(glm::inverse(camera_orientation_inverse), -key_input);

    camera_position -=  key_input * deltaTime * movement_speed;
  }

  const glm::mat4 viewMatrix = camera_orientation_inverse * glm::translate(I, -camera_position);
  this->viewProjectionMatrix = projectionMatrix * viewMatrix;
}


void DebugCamera::operator=(const scene::CameraParameter& cameraParameter)
{
  this->projectionMatrix = cameraParameter.projectionMatrix(windowSize.x, windowSize.y);
  this->camera_orientation_inverse = cameraParameter.viewMatrix();
  this->camera_orientation_inverse[3] = glm::vec4(0,0,0,1);
  this->camera_position = cameraParameter.position;
}


bool DebugCamera::fromJson(const QJsonObject& json, const QMap<QString, scene::CameraParameter>& cameraParameter)
{
  this->locked = QJsonValue(json["locked"]).toBool(false);

  if(json.contains("camera"))
  {
    QString name = QJsonValue(json["camera"]).toString("NOT-A-STRING");
    if(!cameraParameter.contains(name))
    {
      qWarning() << "Unknown camera " << name;
      return false;
    }
    this->loadedName = name;
    *this = cameraParameter[name];
  }
  return true;
}


} // namespace glrt

