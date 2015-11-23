#ifndef GLRT_WINDOW_H
#define GLRT_WINDOW_H

#include "dependencies.h"

namespace glrt {

class System
{
public:
  struct Settings
  {
    sf::String title = "Hello World :)";
    sf::VideoMode videoMode = sf::VideoMode(1920, 1080);
    sf::Uint32 style = sf::Style::Default;
    sf::ContextSettings contextSettings = sf::ContextSettings(24, 8, 0, 4, 5);

    static Settings simpleWindow(const sf::String& title="Hello World :)", int width=1024, int height=768)
    {
      Settings settings;
      settings.title = title;
      settings.videoMode.width = width;
      settings.videoMode.height = height;
      return settings;
    }
  };

  sf::RenderWindow window;

  System(int argc, char** argv, const Settings& settings = Settings::simpleWindow());
};

} // namespace glrt

#endif // GLRT_WINDOW_H
