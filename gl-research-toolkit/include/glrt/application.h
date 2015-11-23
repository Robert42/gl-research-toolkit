#ifndef GLRT_APPLICATION_H
#define GLRT_APPLICATION_H

#include "system.h"
#include "gui/toolbar.h"
#include "profiler.h"

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

  Profiler profiler;
  System system;
  gui::Toolbar gui;
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
