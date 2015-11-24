#ifndef GLRT_DEBUGGUI_H
#define GLRT_DEBUGGUI_H

#include "../dependencies.h"


namespace glrt {
namespace gui {

class Toolbar final
{
public:
  Toolbar();
  ~Toolbar();

  static void registerTweakBar(TwBar* tweakBar);
  static void unregisterTweakBar(TwBar* tweakBar);

  void init();

private:
  TwBar* tweakBar;

  static void setBarVisibility(const bool *value, TwBar* bar);
  static void getBarVisibility(bool *value, TwBar* bar);
};

} // namespace gui
} // namespace glrt

#endif // GLRT_DEBUGGUI_H
