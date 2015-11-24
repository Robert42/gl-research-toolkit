#ifndef GLRT_DEBUGGUI_H
#define GLRT_DEBUGGUI_H

#include "../dependencies.h"


namespace glrt {
namespace gui {

class Toolbar
{
public:
  Toolbar();

  bool handleEvent(const SDL_Event& event);
  void update(float deltaTime);
};

} // namespace gui
} // namespace glrt

#endif // GLRT_DEBUGGUI_H
