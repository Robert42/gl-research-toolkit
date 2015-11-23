#include <glrt/gui/toolbar.h>

#include <SFGUI/Button.hpp>
#include <SFGUI/Window.hpp>

#include <QString>

namespace sfg {
sf::Font LoadDejaVuSansFont();
} // namespace sfg


namespace glrt {
namespace gui {

DebugGui::DebugGui()
  : visible(false),
    fpsVisible(false)
{
  sfg::Window::Ptr debugMenuWindow = sfg::Window::Create();
  debugMenuWindow->SetTitle("GUI [F9]");

  debugMenu = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);

  debugMenu->PackStart(sfg::Button::Create("Show FPS"), false, false);
  debugMenuWindow->Add(debugMenu);
  font = sfg::LoadDejaVuSansFont();

  sfgDesktop.Add(debugMenuWindow);
  initFpsText();
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

  if(fpsVisible)
  {
    if(seconds >= 0.001f)
      fpsText.setString(sf::String(QString("FPS: %0").arg(1.f / seconds).toStdString()));
    else
      fpsText.setString(sf::String(QString("FPS: > 1000").toStdString()));
  }
}

void DebugGui::draw(sf::RenderWindow& renderWindow)
{
  if(visible)
    sfgui.Display(renderWindow);

  if(fpsVisible)
    renderWindow.draw(fpsText);
}

void DebugGui::initFpsText()
{
  fpsText.setFont(font);

  debugMenu->PackStart(createPropertyWidget(&fpsVisible, "Show FPS", BoolStyle::ToggleButton), false, false);
}


} // namespace gui
} // namespace glrt

