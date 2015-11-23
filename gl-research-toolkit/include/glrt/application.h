#ifndef GLRT_WINDOW_H
#define GLRT_WINDOW_H

#include "dependencies.h"

namespace glrt {

class Application
{
public:
  struct Settings
  {
    sf::VideoMode videoMode = sf::VideoMode(1920, 1080);
    sf::String title = "Hello World :)";
    sf::Uint32 style = sf::Style::Default;
    sf::ContextSettings contextSettings = sf::ContextSettings(24, 8, 0, 4, 5);

    static Settings simpleWindow(int width=1024, int height=768)
    {
      Settings settings;
      settings.videoMode.width = width;
      settings.videoMode.height = height;
      return settings;
    }
  };

  sf::RenderWindow window;
  sfg::SFGUI sfgui;
  sfg::Desktop sfgDesktop;
  sfg::Window::Ptr debugMenuWindow;

  Application(int argc, char** argv, const Settings& settings = Settings::simpleWindow());

  bool pollEvent(sf::Event& event);

  void update(float deltaTime);
};

} // namespace glrt

#endif // GLRT_WINDOW_H
