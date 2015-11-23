#ifndef GLRT_APPLICATION_H
#define GLRT_APPLICATION_H

#include "system.h"
#include "debug-gui.h"

namespace glrt {

class Application
{
public:
  struct Settings
  {
    static Settings techDemo()
    {
      Settings settings;
      return settings;
    }
  };

  sf::Clock clock;
  System system;
  DebugGui gui;
  sf::RenderWindow& window;

  float frameDuration;

  Application(int argc, char** argv, const System::Settings& systemSettings = System::Settings::simpleWindow(), const Application::Settings& applicationSettings=Settings::techDemo());

  bool isRunning() const;
  bool pollEvent(sf::Event& event);
  void update();

  void beginDraw();
  void endDraw();
};

} // namespace glrt

#endif // GLRT_APPLICATION_H
