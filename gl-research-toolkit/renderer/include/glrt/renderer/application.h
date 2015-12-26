#ifndef GLRT_RENDERER_APPLICATION_H
#define GLRT_RENDERER_APPLICATION_H

#include <glrt/toolkit/profiler.h>
#include <glrt/scene/debug-camera.h>
#include <glrt/renderer/system.h>

namespace glrt {
namespace renderer {

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
  AngelScript::asIScriptEngine* scriptEngine;

  Profiler profiler;

  bool isRunning;

  Application(int& argc, char** argv, const System::Settings& systemSettings = System::Settings::simpleWindow(), const Application::Settings& applicationSettings=Settings::techDemo());
  ~Application();

  bool pollEvent(SDL_Event* event);
  float update();

  void showWindow();
  void swapWindow();

private:
  bool handleEvent(const SDL_Event& event);
  bool handleWindowEvent(const SDL_WindowEvent& event);
  bool handleKeyPressedEvent(const SDL_KeyboardEvent& event);

  void initAngelScript();
  void deinitAngelScript();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_APPLICATION_H
