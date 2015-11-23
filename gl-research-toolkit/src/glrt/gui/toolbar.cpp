#include <glrt/gui/toolbar.h>

#include <SFGUI/Button.hpp>
#include <SFGUI/Window.hpp>


namespace glrt {
namespace gui {

DebugGui::DebugGui()
  : visible(false)
{
  sfg::Window::Ptr debugMenuWindow = sfg::Window::Create();
  debugMenuWindow->SetTitle("GUI [F9]");

  debugMenu = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);

  debugMenu->PackStart(sfg::Button::Create("Show FPS"), false, false);
  debugMenuWindow->Add(debugMenu);

  sfgDesktop.Add(debugMenuWindow);
}


void DebugGui::handleEvent(const sf::Event& event)
{
  switch(event.type)
  {
  case sf::Event::KeyPressed:
    switch(event.key.code)
    {
    case sf::Keyboard::F1:
      this->visible = !this->visible;
      return;
    case sf::Keyboard::F2:
      this->visible = !this->visible;
      return;
    default:
      break;
    }

    break;
  default:
    break;
  }

  if(visible)
    sfgDesktop.HandleEvent(event);
}

void DebugGui::update(float seconds)
{
  if(visible)
    sfgDesktop.Update(seconds);
}

void DebugGui::draw(sf::RenderWindow& renderWindow)
{
  if(visible)
    sfgui.Display(renderWindow);

}


} // namespace gui
} // namespace glrt

