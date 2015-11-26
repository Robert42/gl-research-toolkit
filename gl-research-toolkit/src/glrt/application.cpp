#include <glrt/application.h>

namespace glrt {


Application::Application(int argc, char** argv, const System::Settings& systemSettings, const Application::Settings& applicationSettings)
  : system(argc, argv, systemSettings),
    settings(applicationSettings),
    sdlWindow(system.sdlWindow),
    isRunning(true),
    showAntTweakBar(applicationSettings.showTweakbarByDefault)
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
  float frameDuration = profiler.update();
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

  if(!settings.sampleDescription.isEmpty())
    TwSetParam(nullptr, nullptr, "help", TW_PARAM_CSTRING, 1, settings.sampleDescription.toStdString().c_str());

  toolbar.init();

  // Create the TweakBar of the application
  if(!settings.tweakBarName.isEmpty())
  {
    this->appTweakBar = TwNewBar(settings.tweakBarName.toStdString().c_str());
    TwSetParam(this->appTweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, settings.tweakBarHelp.toStdString().c_str());

    gui::Toolbar::registerTweakBar(this->appTweakBar);

    qint32 iconified = true;
    TwSetParam(toolbar.tweakBar, nullptr, "iconified", TW_PARAM_INT32, 1, &iconified);
  }else
  {
    this->appTweakBar = nullptr;
  }

  profiler.createTweakBar();
}

void Application::updateAntTweakBarWindowSize()
{
  glm::ivec2 size;

  SDL_GetWindowSize(sdlWindow, &size.x, &size.y);

  TwWindowSize(size.x,
               size.y);
}


} // namespace glrt

