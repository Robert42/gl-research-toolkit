#ifndef GLRT_DEBUGGUI_H
#define GLRT_DEBUGGUI_H

#include "../dependencies.h"

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Desktop.hpp>
#include <SFGUI/Box.hpp>

namespace glrt {
namespace gui {

class DebugGui
{
public:
  sfg::SFGUI sfgui;
  sfg::Desktop sfgDesktop;
  sf::Font font;

  sfg::Box::Ptr debugMenu;

  bool visible : 1;

  DebugGui();

  void handleEvent(const sf::Event& event);
  void update(float seconds);
  void draw(sf::RenderWindow& renderWindow);
};

} // namespace gui
} // namespace glrt

#endif // GLRT_DEBUGGUI_H
