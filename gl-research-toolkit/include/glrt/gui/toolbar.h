#ifndef GLRT_DEBUGGUI_H
#define GLRT_DEBUGGUI_H

#include "../dependencies.h"

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Desktop.hpp>
#include <SFGUI/Box.hpp>

namespace glrt {
namespace gui {

class Toolbar
{
public:
  sfg::SFGUI sfgui;
  sfg::Desktop sfgDesktop;
  sf::Font font;

  sfg::Box::Ptr debugMenu;
  sf::Text fpsText;

  bool visible;
  bool fpsVisible;

  Toolbar();

  void handleEvent(const sf::Event& event);
  void update(float seconds);
  void draw(sf::RenderWindow& renderWindow);

private:
  void initToolbarWindow();
  void initFpsText();
};

} // namespace gui
} // namespace glrt

#endif // GLRT_DEBUGGUI_H
