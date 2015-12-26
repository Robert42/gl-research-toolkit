#include <glrt/toolkit/profiler.h>
#include <glrt/renderer/gui/anttweakbar.h>
#include <glrt/renderer/application.h>
#include <glrt/renderer/scene-renderer.h>
#include <glrt/renderer/debugging/shader-debug-printer.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>

namespace glrt {
namespace renderer {
namespace gui {


AntTweakBar::AntTweakBar(Application* application, const Settings& settings)
  : application(application),
    visible(settings.showByDefault),
    toggleHelp(settings.toggleHelp),
    toggleGui(settings.toggleGui)
{
  Q_ASSERT(application != nullptr);

  TwInit(TW_OPENGL_CORE, NULL);
  updateAntTweakBarWindowSize();

  // Prevent bars to get outside the render window
  TwSetParam(nullptr, nullptr, "contained", TW_PARAM_CSTRING, 1, "true");

  if(!settings.globalDescription.isEmpty())
    TwSetParam(nullptr, nullptr, "help", TW_PARAM_CSTRING, 1, settings.globalDescription.toStdString().c_str());

  toolbar.init();
}


AntTweakBar::~AntTweakBar()
{
  cameraSwitcher.clear();
  sceneSwitcher.clear();

  toolbar.deinit();
  TwTerminate();

}


TwBar* AntTweakBar::createCustomBar(QString name,
                                    QString help,
                                    glm::ivec2 pos,
                                    glm::ivec2 size,
                                    int marginToWindowBorder)
{
  // the defautlt margin (32 px) is exactly the radius of the RotoSlider.
  // By introducing this margin, we are guaranteed to have some space to use
  // the RotoSlider.
  marginToWindowBorder = glm::max(marginToWindowBorder, 0);

  TwBar* tweakBar = TwNewBar(name.toStdString().c_str());
  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, help.toStdString().c_str());
  TwSetParam(tweakBar, nullptr, "size", TW_PARAM_INT32, 2, &size);

  glm::ivec2 windowSize;
  SDL_GetWindowSize(application->sdlWindow, &windowSize.x, &windowSize.y);
  TwGetParam(tweakBar, nullptr, "size", TW_PARAM_INT32, 2, &size);

  pos = glm::clamp(pos, glm::ivec2(marginToWindowBorder), windowSize-size-marginToWindowBorder);
  TwSetParam(tweakBar, nullptr, "position", TW_PARAM_INT32, 2, &pos);

  gui::Toolbar::registerTweakBar(tweakBar);

  return tweakBar;
}


TwBar* AntTweakBar::createProfilerBar(Profiler* profiler)
{
  TwBar* tweakBar = TwNewBar("Profiler");

  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, "Collection of tools to measure the performance.\nNote: For better Performance measurement, you can toggle AntTweakbar with [F9]");

  TwAddVarRW(tweakBar, "Print FPS", TW_TYPE_BOOLCPP, &profiler->printFramerate, "");

  gui::Toolbar::registerTweakBar(tweakBar, true);

  return tweakBar;
}


TwBar* AntTweakBar::createDebugSceneBar(Renderer* renderer)
{
  scene::Scene& scene = renderer->scene;

  connect(&scene, &scene::Scene::sceneLoadedExt, this, &AntTweakBar::handleSceneLoaded);

  TwBar* tweakBar = TwNewBar("Scene");

  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, "Collection of tools to debug a scene.");

  sceneSwitcher = TweakBarEnum<QString>::Ptr(new TweakBarEnum<QString>("CurrentSceneEnum", tweakBar, "Current Scene", ""));
  sceneSwitcher->init(scene::Scene::findAllScenes());
  sceneSwitcher->valueChanged = [&scene](const QString& file){scene.loadFromFile(file);};

  cameraSwitcher = TweakBarEnum<scene::CameraParameter>::Ptr(new TweakBarEnum<scene::CameraParameter>("CurrentCameraEnum", tweakBar, "Current Camera", "group=Camera"));
  cameraSwitcher->valueChanged = [&scene](const scene::CameraParameter& p){scene.debugCamera = p;};
  TwAddVarRW(tweakBar, "Lock Camera", TW_TYPE_BOOLCPP, &scene.debugCamera.locked, "group=Camera");

  renderer->visualizeCameras.guiToggle.TwAddVarCB(tweakBar, "Show Scene Cameras", "group=Debug");
  renderer->visualizeSphereAreaLights.guiToggle.TwAddVarCB(tweakBar, "Show Sphere Area-Lights", "group=Debug");
  renderer->visualizeRectAreaLights.guiToggle.TwAddVarCB(tweakBar, "Show Rect Area-Lights", "group=Debug");

  gui::Toolbar::registerTweakBar(tweakBar, true);

  return tweakBar;
}


void __reload_all_shaders(void*)
{
  ReloadableShader::reloadAll();
}


TwBar* AntTweakBar::createDebugShaderBar(debugging::ShaderDebugPrinter* shaderDebugPrinter)
{
  TwBar* tweakBar = TwNewBar("Shader");

  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, "Collection of tools to debug a shader.");

  TwAddButton(tweakBar, "Reload Shaders", __reload_all_shaders, nullptr, "key=F5 help='Reloads all reloadable shaders'");

  if(shaderDebugPrinter != nullptr)
  {
    shaderDebugPrinter->guiToggle.TwAddVarCB(tweakBar, "Use Printer", "group=Debug key=F6");
    TwAddVarRW(tweakBar, "Clear Scene", TW_TYPE_BOOLCPP, &shaderDebugPrinter->clearScene, "group=Debug");
  }

  gui::Toolbar::registerTweakBar(tweakBar, true);

  return tweakBar;
}

void AntTweakBar::handleSceneLoaded(scene::Scene* scene)
{
  if(sceneSwitcher)
    sceneSwitcher->setCurrentKey(scene->name);

  if(cameraSwitcher)
  {
    cameraSwitcher->init(scene->sceneCameras());
    if(!scene->debugCamera.loadedName.isEmpty())
      cameraSwitcher->setCurrentKey(scene->debugCamera.loadedName);
  }
}


bool AntTweakBar::handleEvents(const SDL_Event& event)
{
  if(visible && TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION))
  {
    handeledEvent(event);
    return true;
  }else
  {
    return unhandeledEvent(event);
  }
}


void AntTweakBar::handeledEvent(const SDL_Event& event)
{
  switch(event.type)
  {
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    if(event.button.button == SDL_BUTTON_LEFT)
    {
      bool captured_mouse = event.button.state==SDL_PRESSED;
      SDL_SetWindowGrab(application->sdlWindow, captured_mouse ? SDL_TRUE : SDL_FALSE);
    }
    return;
  default:
    return;
  }
}


bool AntTweakBar::unhandeledEvent(const SDL_Event& event)
{
  switch(event.type)
  {
  case SDL_MOUSEBUTTONUP:
    if(event.button.button == SDL_BUTTON_LEFT)
      SDL_SetWindowGrab(application->sdlWindow, SDL_FALSE);
    return false;
  case SDL_KEYDOWN:
    switch(event.key.keysym.sym)
    {
    case SDLK_F1:
      if(toggleHelp)
      {
        TwBar* helpBar = TwGetBarByName("TW_HELP");
        qint32 iconified;

        Q_ASSERT(helpBar != nullptr);

        if(visible)
          TwGetParam(helpBar, nullptr, "iconified", TW_PARAM_INT32, 1, &iconified);
        else
          iconified = true;

        iconified = !iconified;

        TwSetParam(helpBar, nullptr, "iconified", TW_PARAM_INT32, 1, &iconified);
        updateAntTweakBarWindowSize();
        return true;
      }
      return false;
    case SDLK_F9:
      if(toggleGui)
      {
        this->visible = !this->visible;
        updateAntTweakBarWindowSize();
        return true;
      }
      return false;
    default:
      return false;
    }

  default:
    return false;
  }
}


void AntTweakBar::draw()
{
  if(visible)
    TwDraw();
}


void AntTweakBar::updateAntTweakBarWindowSize()
{
  glm::ivec2 size;

  SDL_GetWindowSize(application->sdlWindow, &size.x, &size.y);

  TwWindowSize(size.x,
               size.y);
}


} // namespace gui
} // namespace renderer
} // namespace glrt
