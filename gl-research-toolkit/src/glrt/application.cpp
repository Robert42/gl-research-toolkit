#include <glrt/application.h>

#include <glhelper/gl.hpp>

#include <sdk/add_on/scriptstdstring/scriptstdstring.h>

namespace glrt {


Application::Application(int argc, char** argv, const System::Settings& systemSettings, const Application::Settings& applicationSettings)
  : system(argc, argv, systemSettings),
    settings(applicationSettings),
    sdlWindow(system.sdlWindow),
    isRunning(true)
{
  initAngelScript();

  gl::Details::ShaderIncludeDirManager::addIncludeDirs(QDir(GLRT_SHADER_DIR).absoluteFilePath("toolkit"));
  gl::Details::ShaderIncludeDirManager::addIncludeDirs(QDir(GLRT_SHADER_DIR).absoluteFilePath("common-with-cpp"));
  gl::Details::ShaderIncludeDirManager::addIncludeDirs(QDir(GLRT_EXTERNAL_SHADER_DIR).absolutePath());
}


Application::~Application()
{
  deinitAngelScript();
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
  SDL_GL_SwapWindow(sdlWindow);
}


bool Application::handleEvent(const SDL_Event& event)
{
  switch(event.type)
  {
  case SDL_WINDOWEVENT:
    return handleWindowEvent(event.window);
  case SDL_KEYDOWN:
    return handleKeyPressedEvent(event.key);
  case SDL_DROPFILE:
    if(!settings.enableFileDropEvents)
    {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                               "Dropping files not supported",
                               "This Sample accepts no dropped files.",
                               sdlWindow);
      SDL_free(event.drop.file);
      return true;
    }
    return false;
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
  default:
    return false;
  }
}

void Application::initAngelScript()
{
  scriptEngine = AngelScript::asCreateScriptEngine();

  AngelScriptIntegration::init_message_callback_qt(scriptEngine);
  AngelScript::RegisterStdString(scriptEngine);
  AngelScriptIntegration::init_logging_functions_qt(scriptEngine);

  AngelScriptIntegration::init_glm(scriptEngine);
}

void Application::deinitAngelScript()
{
  scriptEngine->ShutDownAndRelease();
}

} // namespace glrt

