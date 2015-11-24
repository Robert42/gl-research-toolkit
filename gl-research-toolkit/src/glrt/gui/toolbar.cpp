#include <glrt/gui/toolbar.h>


namespace glrt {
namespace gui {

Toolbar::Toolbar()
{
}

bool Toolbar::handleEvent(const SDL_Event& event)
{
  Q_UNUSED(event);
  return false;
}

void Toolbar::update(float deltaTime)
{
  Q_UNUSED(deltaTime);
}

} // namespace gui
} // namespace glrt

