#include <glrt/gui/anttweakbar.h>
#include <glrt/profiler.h>
#include <glrt/application.h>
#include <glrt/scene/scene-renderer.h>

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
  TwSetParam(tweakBar, nullptr, "visible", TW_PARAM_CSTRING, 1, "false");

  TwAddVarRW(tweakBar, "Print FPS", TW_TYPE_BOOLCPP, &profiler->printFramerate, "");

  gui::Toolbar::registerTweakBar(tweakBar);

  return tweakBar;
}


TwBar* AntTweakBar::createDebugSceneBar(scene::Renderer* renderer)
{
  TwBar* tweakBar = TwNewBar("Debug Scene");

  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, "Collection of tools to debug a scene.");
  TwSetParam(tweakBar, nullptr, "visible", TW_PARAM_CSTRING, 1, "false");

  renderer->visualizeCameras.guiToggle.TwAddVarCB(tweakBar, "Show Scene Cameras", "");
  renderer->visualizeSphereAreaLights.guiToggle.TwAddVarCB(tweakBar, "Show Sphere Area-Lights", "");
  renderer->visualizeRectAreaLights.guiToggle.TwAddVarCB(tweakBar, "Show Rect Area-Lights", "");

  gui::Toolbar::registerTweakBar(tweakBar);

  return tweakBar;
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
} // namespace glrt
