#ifndef GLRT_DEBUGGUI_H
#define GLRT_DEBUGGUI_H

#include "dependencies.h"

namespace glrt {

class DebugGui
{
public:
  sfg::SFGUI sfgui;
  sfg::Desktop sfgDesktop;
  sfg::Window::Ptr debugMenuWindow;

  DebugGui();

  void handleEvents(const sf::Event& event);
  void update(float seconds);
  void draw(sf::RenderWindow& renderWindow);
};

} // namespace glrt

#endif // GLRT_DEBUGGUI_H
