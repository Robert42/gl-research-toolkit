#ifndef GLRT_DEBUGGUI_H
#define GLRT_DEBUGGUI_H

#include "../dependencies.h"


namespace glrt {
namespace gui {

class Toolbar final
{
public:
  TwBar* tweakBar;

  Toolbar();
  ~Toolbar();

  static void registerTweakBar(TwBar* tweakBar, bool hideNow=false);
  static void unregisterTweakBar(TwBar* tweakBar);

  void init();
  void deinit();

private:
  static void setBarIconified(const bool *value, TwBar* bar);
  static void getBarIconified(bool *value, TwBar* bar);

  static void setBarVisibility(const bool *value, TwBar* bar);
  static void getBarVisibility(bool *value, TwBar* bar);
};

} // namespace gui
} // namespace glrt

#endif // GLRT_DEBUGGUI_H
