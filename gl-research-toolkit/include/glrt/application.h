#ifndef GLRT_APPLICATION_H
#define GLRT_APPLICATION_H

#include "system.h"
#include "profiler.h"
#include "debug-camera.h"
#include "toolkit/antifreeze.h"

namespace glrt {

class Application final
{
public:
  struct Settings final
  {
    bool quitWithEscape = false;
    bool enableFileDropEvents = false;

    static Settings techDemo(bool enableFileDropEvents = false)
    {
      Settings settings;
      settings.quitWithEscape = true;
      settings.enableFileDropEvents = enableFileDropEvents;
      return settings;
    }
  };

  System system;
  Settings settings;
  SDL_Window* const sdlWindow;
  asIScriptEngine* scriptEngine;

  Profiler profiler;

  bool isRunning;

  Application(int argc, char** argv, const System::Settings& systemSettings = System::Settings::simpleWindow(), const Application::Settings& applicationSettings=Settings::techDemo());
  ~Application();

  bool pollEvent(SDL_Event* event);
  float update();

  void swapWindow();

private:
  Antifreeze antifreeze;

  bool handleEvent(const SDL_Event& event);
  bool handleWindowEvent(const SDL_WindowEvent& event);
  bool handleKeyPressedEvent(const SDL_KeyboardEvent& event);

  void initAngelScript();
  void deinitAngelScript();
};

} // namespace glrt

#endif // GLRT_APPLICATION_H
