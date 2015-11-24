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
    // if the event wasn't handeled, return true to tell the real application
    // that polling was successful and the event can be handeled now.
    if(!handleEvent(event))
      return true;
  }

  // There are no events, for the real application to handle
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


bool Application::handleEvent(const SDL_Event& event)
{
  if(gui.handleEvent(event))
    return true;

  switch(event.type)
  {
  case SDL_WINDOWEVENT:
    return handleWindowEvent(event.window);
  default:
    return false;
  }
}


bool Application::handleWindowEvent(const SDL_WindowEvent& event)
{
  switch(event.type)
  {
  case SDL_WINDOWEVENT_CLOSE:
    return true;
  default:
    return false;
  }
}


} // namespace glrt

