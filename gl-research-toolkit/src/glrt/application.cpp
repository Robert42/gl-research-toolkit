#include <glrt/application.h>

namespace glrt {


Application::Application(int argc, char** argv, const System::Settings& systemSettings, const Application::Settings& applicationSettings)
  : system(argc, argv, systemSettings),
    settings(applicationSettings),
    sdlWindow(system.sdlWindow),
    isRunning(true),
    showAntTweakBar(false)
{
  initAntTweakBar();
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
  if(showAntTweakBar)
    TwDraw();
  SDL_GL_SwapWindow(sdlWindow);
}


bool Application::handleEvent(const SDL_Event& event)
{
  if(showAntTweakBar && TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION))
    return true;

  switch(event.type)
  {
  case SDL_WINDOWEVENT:
    return handleWindowEvent(event.window);
  case SDL_KEYDOWN:
    return handleKeyPressedEvent(event.key);
  default:
    return false;
  }
}


bool Application::handleWindowEvent(const SDL_WindowEvent& event)
{
  switch(event.event)
  {
  case SDL_WINDOWEVENT_CLOSE:
    this->isRunning = false;
    return true;
  default:
    return false;
  }
}


bool Application::handleKeyPressedEvent(const SDL_KeyboardEvent& event)
{
  switch(event.keysym.sym)
  {
  case SDLK_ESCAPE:
    if(settings.quitWithEscape)
    {
      this->isRunning = false;
      return true;
    }
    return false;
  case SDLK_F9:
    this->showAntTweakBar = !this->showAntTweakBar;
    updateAntTweakBarWindowSize();
    return true;
  default:
    return false;
  }
}


void Application::initAntTweakBar()
{
  TwInit(TW_OPENGL_CORE, NULL);
  updateAntTweakBarWindowSize();
}

void Application::updateAntTweakBarWindowSize()
{
  glm::ivec2 size;

  SDL_GetWindowSize(sdlWindow, &size.x, &size.y);

  TwWindowSize(size.x,
               size.y);
}


} // namespace glrt

