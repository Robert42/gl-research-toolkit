#include <glrt/gui/anttweakbar.h>
#include <glrt/profiler.h>
#include <glrt/application.h>


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
                                    const glm::ivec2& pos,
                                    const glm::ivec2& size)
{
  TwBar* tweakBar = TwNewBar(name.toStdString().c_str());
  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, help.toStdString().c_str());
  TwSetParam(tweakBar, nullptr, "position", TW_PARAM_INT32, 2, &pos);
  TwSetParam(tweakBar, nullptr, "size", TW_PARAM_INT32, 2, &size);

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


bool AntTweakBar::handleEvents(const SDL_Event& event)
{
  if(visible && TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION))
    return true;

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
