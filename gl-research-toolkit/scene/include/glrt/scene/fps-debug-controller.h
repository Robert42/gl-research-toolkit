#ifndef GLRT_DEBUGCAMERA_H
#define GLRT_DEBUGCAMERA_H

#include <glrt/dependencies.h>

#include <glrt/scene/node.h>
#include <glrt/scene/camera-parameter.h>
#include <glrt/scene/input-handler.h>
#include <glrt/scene/tick-manager.h>
#include <glrt/scene/camera-component.h>

namespace glrt {
namespace scene {

class FpsDebugInputHandler final : public InputHandler
{
public:
  static bool locked;
  CoordFrame frame;

  float movement_speed;
  float rotation_speed;
  QString loadedName;

  FpsDebugInputHandler();

  bool handleEvent(const SDL_Event& event) override;

  void update(float deltaTime);

private:
  bool movementMode;
};

class FpsDebugController final : public TickNodeAttribute
{
public:
  FpsDebugInputHandler inputHandler;
  CameraComponent& component;

  FpsDebugController(CameraComponent& component, const Uuid<FpsDebugController>& uuid);

  void tick(float timeDelta) override;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_DEBUGCAMERA_H
