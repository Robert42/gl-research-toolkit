#include <glrt/toolkit/profiler.h>
#include <glrt/application.h>
#include <glrt/gui/anttweakbar.h>
#include <glrt/renderer/scene-renderer.h>
#include <glrt/renderer/debugging/shader-debug-printer.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>

namespace glrt {
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
  visualizationSwitcher.clear();

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
  TwSetParam(tweakBar, nullptr, "size", TW_PARAM_CSTRING, 1, "320 320");

  toggleProfiler.getter = [profiler]()->bool{return profiler->isActive();};
  toggleProfiler.setter = [profiler](bool a){
    if(a)
      profiler->activate();
    else
      profiler->deactivate();
  };
  toggleProfiler.TwAddVarCB(tweakBar, "Connect", "key=F10 help='Connect with the profiler gui (which must be already running)'");

  gui::Toolbar::registerTweakBar(tweakBar, true);

  return tweakBar;
}


TwBar* AntTweakBar::createDebugSceneBar(renderer::Renderer* renderer)
{
  scene::Scene& scene = renderer->scene;

  connect(&scene, &scene::Scene::sceneLoadedExt, this, &AntTweakBar::handleSceneLoaded);

  TwBar* tweakBar = TwNewBar("Scene");

  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, "Collection of tools to debug a scene.");
  TwSetParam(tweakBar, nullptr, "size", TW_PARAM_CSTRING, 1, "320 320");

  sceneSwitcher = SceneEnumeration::Ptr(new SceneEnumeration("CurrentSceneEnum", tweakBar, "Current Scene", ""));
  sceneSwitcher->initWithUuids<Scene>(scene.resourceManager, scene.resourceManager.allRegisteredScenes());
  sceneSwitcher->valueChangedByUser = [&scene](const Uuid<Scene>& uuid){scene.load(uuid);};

  cameraSwitcher = CameraEnumeration::Ptr(new CameraEnumeration("CurrentCameraEnum", tweakBar, "Current Camera", "group=Camera"));
  cameraSwitcher->valueChanged = [&scene](const QPointer<scene::CameraComponent>& otherCamera){switchDebugCameraTo(&scene, otherCamera);};
  TwAddVarRW(tweakBar, "Lock Camera", TW_TYPE_BOOLCPP, &scene::FpsDebugInputHandler::locked, "group=Camera");

  renderer->visualizeCameras.guiToggle.TwAddVarCB(tweakBar, "Show Scene Cameras", "group=Debug");
  renderer->visualizeSphereAreaLights.guiToggle.TwAddVarCB(tweakBar, "Show Sphere Area-Lights", "group=Debug");
  renderer->visualizeRectAreaLights.guiToggle.TwAddVarCB(tweakBar, "Show Rect Area-Lights", "group=Debug");

  renderer->visualizePosteffect_OrangeTest.guiToggle.TwAddVarCB(tweakBar, "Orange CommandList Test", "group=Debug");

  gui::Toolbar::registerTweakBar(tweakBar, true);

  return tweakBar;
}


void __reload_all_shaders(void*)
{
  renderer::ReloadableShader::reloadAll();
}


TwBar* AntTweakBar::createDebugShaderBar(renderer::Renderer* renderer, renderer::debugging::ShaderDebugPrinter* shaderDebugPrinter)
{
  TwBar* tweakBar = TwNewBar("Shader");

  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, "Collection of tools to debug a shader.");
  TwSetParam(tweakBar, nullptr, "size", TW_PARAM_CSTRING, 1, "320 320");

  TwAddButton(tweakBar, "Reload Shaders", __reload_all_shaders, nullptr, "key=F5 help='Reloads all reloadable shaders'");

  roughnessAdjustmentToggle.setter = [renderer](bool ar){renderer->setAdjustRoughness(ar);};
  roughnessAdjustmentToggle.getter = [renderer]() -> bool {return renderer->adjustRoughness();};
  roughnessAdjustmentToggle.TwAddVarCB(tweakBar, "Roughness Adjustment", "group=PBS");

  if(shaderDebugPrinter != nullptr)
  {
    shaderDebugPrinter->guiToggle.TwAddVarCB(tweakBar, "Use Printer", "group=Debug key=F6");
    TwAddVarRW(tweakBar, "Clear Scene", TW_TYPE_BOOLCPP, &shaderDebugPrinter->clearScene, "group=Debug");
  }

  visualizationSwitcher = VisualizationEnumeration::Ptr(new VisualizationEnumeration("VisualizationEnumeration", tweakBar, "Shader Visualization", "keyincr=F7 keydecr=SHIFT+F7 help='The Surface Shader vizualization'"));
  visualizationSwitcher->init(glrt::renderer::allSurfaceShaderVisualizations());
  visualizationSwitcher->setCurrentValue(glrt::renderer::currentSurfaceShaderVisualization);
  visualizationSwitcher->valueChanged = [](glrt::renderer::SurfaceShaderVisualization visualization){glrt::renderer::setCurrentSurfaceShaderVisualization(visualization);};

  gui::Toolbar::registerTweakBar(tweakBar, true);

  return tweakBar;
}

void AntTweakBar::switchDebugCameraTo(Scene* scene, const QPointer<scene::CameraComponent>& otherCamera)
{
  QPointer<scene::CameraComponent> debugCamera = scene::findDebugCameraComponent(scene);
  scene::FpsDebugController* fpsController = scene::findFpsDebugController(scene);

  if(!debugCamera.isNull() && !otherCamera.isNull())
  {
    debugCamera->set_localCoordFrame(otherCamera->localCoordFrame());
    debugCamera->cameraParameter = otherCamera->cameraParameter;

    if(fpsController)
      fpsController->inputHandler.frame = otherCamera->localCoordFrame();
  }
}

void AntTweakBar::handleSceneLoaded(scene::Scene* scene)
{
  if(sceneSwitcher)
    sceneSwitcher->setCurrentValue(scene->uuid);

  if(cameraSwitcher)
  {
    QMap<QString, QPointer<scene::CameraComponent>> cameras;
    QHash<QString, scene::CameraComponent*> h = glrt::scene::collectAllComponentsWithTypeNamed<scene::CameraComponent>(scene);
    for(auto i=h.begin(); i!=h.end(); ++i)
    {
      if(i.value()->uuid != scene::uuids::debugCameraComponent)
        cameras.insert(i.key(), i.value());
    }
    cameraSwitcher->init(cameras);
    if(!cameras.isEmpty())
      cameraSwitcher->setCurrentKey(cameras.keys().first());

    QPointer<scene::CameraComponent> cameraComponent = glrt::scene::findComponent(scene, scene->camera(glrt::scene::CameraSlot::MAIN_CAMERA));
    if(!cameraComponent.isNull())
      cameraSwitcher->setCurrentValue(cameraComponent);

  }
}


bool AntTweakBar::handleEvents(const SDL_Event& event)
{
  PROFILE_SCOPE("AntTweakBar (Events)")

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
      // Workaround:
      // Don't capture the mouse immediatly, as if the application freezes,
      // (because of an error while handling a pressed button),
      // the mouse should not be captured so the user can use other applications
      // As the event is lgging behind, wait 20 frames before capturing the mouse
      captureMouseDeferred = captured_mouse ? MouseCaptureState(20) : MouseCaptureState::IDLE;
      SDL_SetWindowGrab(application->sdlWindow, SDL_FALSE);
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

// The mouse wasn't captured immediatly, as if the application freezes, because of an error while handling a pressed button, the mouse should not be captured during debugging
inline void AntTweakBar::deferredMouseCapture()
{
  if(captureMouseDeferred==MouseCaptureState::CAPTURE_NOW && this->visible)
  {
    captureMouseDeferred = MouseCaptureState::IDLE;
    SDL_SetWindowGrab(application->sdlWindow, SDL_TRUE);
  }else if(captureMouseDeferred>=MouseCaptureState::CAPTURE_NEXT_FRAME)
  {
    captureMouseDeferred = MouseCaptureState(int(captureMouseDeferred)-1);
  }
}


void AntTweakBar::draw()
{
  PROFILE_SCOPE("AntTweakBar (Draw)")

  if(visible)
    TwDraw();

  deferredMouseCapture();
}


void AntTweakBar::updateAntTweakBarWindowSize()
{
  glm::ivec2 size;

  SDL_GetWindowSize(application->sdlWindow, &size.x, &size.y);

  TwWindowSize(size.x,
               size.y);
}


} // namespace gui
} // namespace glrt
