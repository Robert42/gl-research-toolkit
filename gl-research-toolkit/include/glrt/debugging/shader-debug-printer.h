#ifndef GLRT_DEBUGGING_SHADERDEBUGPRINTER_H
#define GLRT_DEBUGGING_SHADERDEBUGPRINTER_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>
#include <glrt/gui/anttweakbar.h>

namespace glrt {
namespace debugging {

class ShaderDebugPrinter final
{
public:
  gui::TweakBarCBVar<bool> guiToggle;

  ShaderDebugPrinter();
  ~ShaderDebugPrinter();

  bool handleEvents(const SDL_Event& event);

  void begin();
  void end();

  void drawCross();

private:
  bool active = false;
  bool mouse_is_pressed = false;
  gl::Buffer buffer;
};

} // namespace debugging
} // namespace glrt

#endif // GLRT_DEBUGGING_SHADERDEBUGPRINTER_H
