#ifndef GLRT_APPLICATION_H
#define GLRT_APPLICATION_H

#include "system.h"
#include "gui/toolbar.h"
#include "profiler.h"

namespace glrt {

class Application final
{
public:
  struct Settings final
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
  SDL_Window* const sdlWindow;

  Profiler profiler;
  gui::Toolbar gui;

  float frameDuration;
  bool isRunning;
  bool showAntTweakBar;

  Application(int argc, char** argv, const System::Settings& systemSettings = System::Settings::simpleWindow(), const Application::Settings& applicationSettings=Settings::techDemo());

  bool pollEvent(SDL_Event* event);
  float update();

  void swapWindow();

private:
  bool handleEvent(const SDL_Event& event);
  bool handleWindowEvent(const SDL_WindowEvent& event);
  bool handleKeyPressedEvent(const SDL_KeyboardEvent& event);

  void initAntTweakBar();
  void updateAntTweakBarWindowSize();
};

} // namespace glrt

#endif // GLRT_APPLICATION_H
