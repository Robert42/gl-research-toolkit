#include <glrt/scene/fps-debug-controller.h>
#include <glrt/scene/scene-layer.h>
#include <glrt/scene/scene.h>
#include <glrt/toolkit/geometry.h>

namespace glrt {
namespace scene {

FpsDebugInputHandler::FpsDebugInputHandler()
{
  movementMode = false;

  movement_speed = 5.f;
  rotation_speed = glm::radians(1.f);
  locked = false;
}

bool FpsDebugInputHandler::handleEvent(const SDL_Event& event)
{
  if(locked)
  {
    movementMode = false;
    return false;
  }

  const glm::vec3 x(1, 0, 0);
  const glm::vec3 y(0, 1, 0);
  const glm::vec3 z(0, 0, 1);

  Q_UNUSED(x);
  Q_UNUSED(y);
  Q_UNUSED(z);

  switch(event.type)
  {
  case SDL_MOUSEMOTION:
    if(movementMode)
    {
      const glm::vec2 angle = rotation_speed * glm::vec2(event.motion.xrel, event.motion.yrel);

      frame.orientation = glm::angleAxis(-angle.x, z) * frame.orientation * glm::angleAxis(-angle.y, x);
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
    return false;
  default:
    return false;
  }
}

void FpsDebugInputHandler::update(float deltaTime)
{
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

    key_input = frame.transform_direction(key_input);

    frame.position +=  key_input * deltaTime * movement_speed;
  }
}

// =============================================================================

FpsDebugController::FpsDebugController(Node::Component& component, const Uuid<FpsDebugController>& uuid)
  : Node::ModularAttribute(component.node, uuid),
    component(component)
{
  connect(&component, &Node::Component::destroyed, this, &FpsDebugController::deleteLater);

  node.sceneLayer.scene.inputManager.addHandler(&inputHandler);

  inputHandler.frame = component.localCoordFrame();
}

void FpsDebugController::tick(float timeDelta)
{
  inputHandler.update(timeDelta);

  component.set_localCoordFrame(inputHandler.frame);
}

TickingObject::TickTraits FpsDebugController::tickTraits() const
{
  return TickTraits::OnlyMainThread;
}

void FpsDebugController::collectTickDependencies(TickDependencySet* dependencySet) const
{
  dependencySet->addDependency(&component);
}


} // namespace scene
} // namespace glrt

