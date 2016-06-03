#ifndef GLRT_GUI_TOOLBAR_H
#define GLRT_GUI_TOOLBAR_H

#include <glrt/renderer/dependencies.h>


namespace glrt {

class Application;

namespace gui {

class Toolbar final
{
public:
  TwBar* tweakBar;
  float fps;

  Toolbar();
  ~Toolbar();

  static void registerTweakBar(TwBar* tweakBar, bool hideNow=false);
  static void unregisterTweakBar(TwBar* tweakBar);

  void init(Application* application);
  void deinit();

private:
  static void setBarIconified(const bool *value, TwBar* bar);
  static void getBarIconified(bool *value, TwBar* bar);

  static void setBarVisibility(const bool *value, TwBar* bar);
  static void getBarVisibility(bool *value, TwBar* bar);
};

} // namespace gui
} // namespace glrt

#endif // GLRT_GUI_TOOLBAR_H
