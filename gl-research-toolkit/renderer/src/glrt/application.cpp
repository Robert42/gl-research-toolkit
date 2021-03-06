#include <glrt/application.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/resources/static-mesh-loader.h>
#include <glrt/scene/resources/asset-converter.h>
#include <glrt/renderer/toolkit/shader-compiler.h>

#include <glhelper/gl.hpp>
#include <QThread>

#include <assimp/DefaultLogger.hpp>

#include <sdk/add_on/scriptstdstring/scriptstdstring.h>
#include <sdk/add_on/scriptarray/scriptarray.h>
#include <sdk/add_on/scriptdictionary/scriptdictionary.h>

#define SLOW_DOWN_IN_DEBUG 500

namespace glrt {


Application::Application(int& argc, char** argv, const System::Settings& systemSettings, const Application::Settings& applicationSettings)
  : system(argc, argv, systemSettings),
    settings(applicationSettings),
    sdlWindow(system.sdlWindow),
    profiler(systemSettings.windowTitle),
    isRunning(true),
    hasFocus(true)
{
  initAngelScript();
  initAssimp();

  glrt::renderer::ShaderCompiler::registerGlrtShaderIncludeDirectories();
}


Application::~Application()
{
  deinitAssimp();
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
#if defined(QT_DEBUG) && defined(SLOW_DOWN_IN_DEBUG)
  if(Q_UNLIKELY(!hasFocus))
    QThread::msleep(SLOW_DOWN_IN_DEBUG);
#endif
  qApp->processEvents();
  frameDuration = profiler.update();
  frameDurationMS = frameDuration * 1000.f;
  frameRate = 1.f / frameDuration;
  return frameDuration;
}


void Application::showWindow()
{
  system.showWindow();

  shaderRecompileWorkaround();
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
  case SDL_WINDOWEVENT_FOCUS_GAINED:
    this->hasFocus = true;
    return false;
  case SDL_WINDOWEVENT_FOCUS_LOST:
    this->hasFocus = false;
    return false;
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
  glrt::angelScriptEngine = this->scriptEngine = AngelScript::asCreateScriptEngine();

  this->scriptEngine->SetEngineProperty(AngelScript::asEP_REQUIRE_ENUM_SCOPE, 1);

  AngelScriptIntegration::init_message_callback_qt(scriptEngine);

  asDWORD previousMask = scriptEngine->SetDefaultAccessMask(AngelScriptIntegration::ACCESS_MASK_ALL);
  AngelScript::RegisterStdString(scriptEngine);
  AngelScript::RegisterScriptArray(scriptEngine, true);
  AngelScript::RegisterScriptDictionary(scriptEngine);
  scriptEngine->SetDefaultAccessMask(previousMask);

  AngelScriptIntegration::init_glm(scriptEngine, AngelScriptIntegration::GlmFlags::NO_SWIZZLE);

  AngelScriptIntegration::init_logging_functions_qt(scriptEngine);


  glrt::Uuid<void>::registerAngelScriptAPI();
  glrt::scene::CoordFrame::registerAngelScriptAPIDeclarations();
  glrt::Uuid<void>::registerCustomizedUuidType("Texture", true);
  glrt::Uuid<void>::registerCustomizedUuidType("StaticMesh", true);
  glrt::Uuid<void>::registerCustomizedUuidType("CameraParameter", true);
  glrt::scene::CoordFrame::registerAngelScriptAPI();
  glrt::scene::resources::StaticMeshLoader::registerAngelScriptAPI();
  glrt::scene::resources::ResourceIndex::registerAngelScriptAPI();
}

void Application::deinitAngelScript()
{
  scriptEngine->ShutDownAndRelease();
  glrt::angelScriptEngine = this->scriptEngine = nullptr;
}

void Application::initAssimp()
{
  class QDebugLogger : public Assimp::Logger
  {
  public:
    void OnDebug(const char* message) override
    {
      qDebug() << "Assimp: " << message;
    }
    void OnInfo(const char* message) override
    {
      qInfo() << "Assimp: " << message;
    }
    void OnWarn(const char* message) override
    {
      qWarning() << "Assimp: " << message;
    }
    void OnError(const char* message) override
    {
      qCritical() << "Assimp: " << message;
    }
    bool attachStream(Assimp::LogStream*,unsigned int) override
    {
      return false;
    }
    bool detatchStream(Assimp::LogStream*,unsigned int) override
    {
      return false;
    }
  };
  Assimp::DefaultLogger::set(new QDebugLogger);
}

void Application::deinitAssimp()
{
}

} // namespace glrt

