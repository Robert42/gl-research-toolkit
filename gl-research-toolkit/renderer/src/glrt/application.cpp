#include <glrt/application.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/resources/static-mesh-loader.h>
#include <glrt/scene/resources/asset-converter.h>

#include <glhelper/gl.hpp>

#include <assimp/DefaultLogger.hpp>

#include <sdk/add_on/scriptstdstring/scriptstdstring.h>
#include <sdk/add_on/scriptarray/scriptarray.h>
#include <sdk/add_on/scriptdictionary/scriptdictionary.h>

namespace glrt {


Application::Application(int& argc, char** argv, const System::Settings& systemSettings, const Application::Settings& applicationSettings)
  : system(argc, argv, systemSettings),
    settings(applicationSettings),
    sdlWindow(system.sdlWindow),
    isRunning(true)
{
  initAngelScript();
  initAssimp();

  gl::Details::ShaderIncludeDirManager::addIncludeDirs(QDir(GLRT_SHADER_DIR).absoluteFilePath("toolkit"));
  gl::Details::ShaderIncludeDirManager::addIncludeDirs(QDir(GLRT_SHADER_DIR).absoluteFilePath("common-with-cpp"));
  gl::Details::ShaderIncludeDirManager::addIncludeDirs(QDir(GLRT_EXTERNAL_SHADER_DIR).absolutePath());
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
  float frameDuration = profiler.update();
  return frameDuration;
}


void Application::showWindow()
{
  system.showWindow();
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
  glrt::Uuid<void>::registerCustomizedUuidType("StaticMesh", true);
  glrt::Uuid<void>::registerCustomizedUuidType("Camera", true);
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

