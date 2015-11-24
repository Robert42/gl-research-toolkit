#include <glrt/application.h>

namespace glrt {


Application::Application(int argc, char** argv, const System::Settings& systemSettings, const Application::Settings& applicationSettings)
  : system(argc, argv, systemSettings),
    sdlWindow(system.sdlWindow)
{
  Q_UNUSED(applicationSettings);
}


bool Application::isRunning() const
{
  return true;
}


bool Application::pollEvent(SDL_Event* e)
{
  SDL_Event& event = *e;

  while(SDL_PollEvent(&event))
  {
    if(gui.handleEvent(event))
      continue;

    return true;
  }

  return false;
}


float Application::update()
{
  frameDuration = profiler.update();
  gui.update(frameDuration);
  return frameDuration;
}


void Application::swapWindow()
{
  SDL_GL_SwapWindow(sdlWindow);
}


} // namespace glrt

