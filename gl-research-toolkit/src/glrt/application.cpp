#include <glrt/application.h>

namespace glrt {


Application::Application(int argc, char** argv, const System::Settings& systemSettings, const Application::Settings& applicationSettings)
  : system(argc, argv, systemSettings),
    window(system.window)
{
  Q_UNUSED(applicationSettings);

  window.pushGLStates();
  window.resetGLStates();
}


bool Application::isRunning() const
{
  return window.isOpen();
}


bool Application::pollEvent(sf::Event& event)
{
  while(window.pollEvent(event))
  {
    gui.handleEvent(event);

    switch(event.type)
    {
    case sf::Event::Closed:
      window.close();
      break;
    case sf::Event::KeyPressed:
      switch(event.key.code)
      {
      case sf::Keyboard::Escape:
        window.close();
        continue;
      default:
        break;
      }
      break;
    default:
      break;
    }

    return true;
  }

  return false;
}


void Application::update()
{
  frameDuration = profiler.update();
  gui.update(frameDuration);
}


void Application::beginDraw()
{
  window.clear();
}


void Application::endDraw()
{
  gui.draw(window);
  window.display();
}


} // namespace glrt

