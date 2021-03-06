#include <glrt/gui/toolbar.h>
#include <glrt/application.h>


namespace glrt {
namespace gui {

const char* toolbarTitle = "Toolbar";

const bool toggleVisibility = false;

Toolbar::Toolbar()
  : tweakBar(nullptr)
{
}


Toolbar::~Toolbar()
{
  Q_ASSERT_X(tweakBar == nullptr, "glrt::gui::Toolbar::~Toolbar()", "You forgot to call Toolbar::deinit()");
}

void Toolbar::init(Application* application)
{
  glm::ivec2 pos(4096);
  glm::ivec2 size(200, 128);

  float refreshRate = 0.25f;

  tweakBar = TwNewBar(toolbarTitle);
  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, "Toggles the visibilty of other tweak bars");
  TwSetParam(tweakBar, nullptr, "position", TW_PARAM_INT32, 2, &pos);
  TwSetParam(tweakBar, nullptr, "size", TW_PARAM_INT32, 2, &size);
  TwSetParam(tweakBar, nullptr, "refresh", TW_PARAM_FLOAT, 1, &refreshRate);

  TwAddVarRO(tweakBar, "FPS", TW_TYPE_FLOAT, &application->frameRate, "readonly=true help='the framerate of the application'");
  TwAddVarRO(tweakBar, "ms", TW_TYPE_FLOAT, &application->frameDurationMS, "readonly=true help='the frameduration of the application in milliseconds'");
}

void Toolbar::deinit()
{
  if(tweakBar != nullptr)
    TwDeleteBar(tweakBar);
  tweakBar = nullptr;
}

void Toolbar::registerTweakBar(TwBar *tweakBar, bool hideNow)
{
  TwBar* toolTweakBar = TwGetBarByName(toolbarTitle);

  Q_ASSERT(toolTweakBar != nullptr);

  if(toolTweakBar == nullptr)
    return;

  if(toggleVisibility)
  {
    TwAddVarCB(toolTweakBar,
               TwGetBarName(tweakBar),
               TW_TYPE_BOOLCPP,
               reinterpret_cast<TwSetVarCallback>(setBarVisibility),
               reinterpret_cast<TwGetVarCallback>(getBarVisibility),
               tweakBar,
               "");
  }else
  {
    TwAddVarCB(toolTweakBar,
               TwGetBarName(tweakBar),
               TW_TYPE_BOOLCPP,
               reinterpret_cast<TwSetVarCallback>(setBarIconified),
               reinterpret_cast<TwGetVarCallback>(getBarIconified),
               tweakBar,
               "");
  }

  if(hideNow)
    TwSetParam(tweakBar, nullptr, toggleVisibility ? "visible" : "iconified", TW_PARAM_CSTRING, 1, "true");
}


void Toolbar::unregisterTweakBar(TwBar* tweakBar)
{
  TwBar* toolTweakBar = TwGetBarByName(toolbarTitle);

  if(toolTweakBar == nullptr)
    return;

  TwRemoveVar(toolTweakBar, TwGetBarName(tweakBar));
}


void Toolbar::setBarVisibility(const bool *value, TwBar* bar)
{
  qint32 visible = *value;
  TwSetParam(bar, nullptr, "visible", TW_PARAM_INT32, 1, &visible);
}


void Toolbar::getBarVisibility(bool *value, TwBar* bar)
{
  qint32 visible = 0;
  TwGetParam(bar, nullptr, "visible", TW_PARAM_INT32, 1, &visible);
  *value = visible;
}


void Toolbar::setBarIconified(const bool *value, TwBar* bar)
{
  qint32 iconified = !*value;
  TwSetParam(bar, nullptr, "iconified", TW_PARAM_INT32, 1, &iconified);
}


void Toolbar::getBarIconified(bool *value, TwBar* bar)
{
  qint32 iconified = 0;
  TwGetParam(bar, nullptr, "iconified", TW_PARAM_INT32, 1, &iconified);
  *value = !iconified;
}


} // namespace gui
} // namespace glrt

