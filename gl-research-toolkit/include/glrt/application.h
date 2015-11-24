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
  SDL_Window* const sdlWindow;

  float frameDuration;

  Application(int argc, char** argv, const System::Settings& systemSettings = System::Settings::simpleWindow(), const Application::Settings& applicationSettings=Settings::techDemo());

  bool isRunning() const;
  bool pollEvent(SDL_Event* event);
  float update();

  void swapWindow();

private:
  bool handleEvent(const SDL_Event& event);
  bool handleWindowEvent(const SDL_WindowEvent& event);
};

} // namespace glrt

#endif // GLRT_APPLICATION_H
