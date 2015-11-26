#include <glrt/gui/toolbar.h>


namespace glrt {
namespace gui {

const char* toolbarTitle = "Toolbar";

Toolbar::Toolbar()
  : tweakBar(nullptr)
{
}


Toolbar::~Toolbar()
{
  if(tweakBar != nullptr)
    TwDeleteBar(tweakBar);
}

void Toolbar::init()
{
  glm::ivec2 pos(4096);
  glm::ivec2 size(200, 1);

  tweakBar = TwNewBar(toolbarTitle);
  TwSetParam(tweakBar, nullptr, "help", TW_PARAM_CSTRING, 1, "Toggles the visibilty of other tweak bars");
  TwSetParam(tweakBar, nullptr, "position", TW_PARAM_INT32, 2, &pos);
  TwSetParam(tweakBar, nullptr, "size", TW_PARAM_INT32, 2, &size);
}

void Toolbar::registerTweakBar(TwBar *tweakBar)
{
  TwBar* toolTweakBar = TwGetBarByName(toolbarTitle);

  Q_ASSERT(toolTweakBar != nullptr);

  if(toolTweakBar == nullptr)
    return;

  TwAddVarCB(toolTweakBar,
             TwGetBarName(tweakBar),
             TW_TYPE_BOOLCPP,
             reinterpret_cast<TwSetVarCallback>(setBarVisibility),
             reinterpret_cast<TwGetVarCallback>(getBarVisibility),
             tweakBar,
             "");
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


} // namespace gui
} // namespace glrt

