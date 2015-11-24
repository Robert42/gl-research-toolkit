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
    bool quitWithEscape = false;

    static Settings techDemo()
    {
      Settings settings;
      settings.quitWithEscape = true;
      return settings;
    }
  };

  System system;
  Settings settings;
  Profiler profiler;
  gui::Toolbar gui;
  SDL_Window* const sdlWindow;

  float frameDuration;
  bool isRunning;

  Application(int argc, char** argv, const System::Settings& systemSettings = System::Settings::simpleWindow(), const Application::Settings& applicationSettings=Settings::techDemo());

  bool pollEvent(SDL_Event* event);
  float update();

  void swapWindow();

private:
  bool handleEvent(const SDL_Event& event);
  bool handleWindowEvent(const SDL_WindowEvent& event);
  bool handleKeyPressedEvent(const SDL_KeyboardEvent& event);
};

} // namespace glrt

#endif // GLRT_APPLICATION_H
