#include <glrt/debug-gui.h>

#include <SFGUI/Button.hpp>

namespace glrt {

DebugGui::DebugGui()
{
  debugMenuWindow = sfg::Window::Create();
  debugMenuWindow->SetTitle("Debug Menu");
  debugMenuWindow->Add(sfg::Button::Create());

  sfgDesktop.Add(debugMenuWindow);
}


void DebugGui::handleEvents(const sf::Event& event)
{
  sfgDesktop.HandleEvent(event);
}

void DebugGui::update(float seconds)
{
  sfgDesktop.Update(seconds);
}

void DebugGui::draw(sf::RenderWindow& renderWindow)
{
  sfgui.Display(renderWindow);
}


} // namespace glrt

