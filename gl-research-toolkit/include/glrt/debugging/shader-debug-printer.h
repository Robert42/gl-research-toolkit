#ifndef GLRT_DEBUGGING_SHADERDEBUGPRINTER_H
#define GLRT_DEBUGGING_SHADERDEBUGPRINTER_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>

namespace glrt {
namespace debugging {

class ShaderDebugPrinter final
{
public:
  bool active = false;

  ShaderDebugPrinter();
  ~ShaderDebugPrinter();

  void begin();
  void end();

  bool handleEvents(const SDL_Event& event);

private:
  bool mouse_is_pressed = false;
  gl::Buffer buffer;
};

} // namespace debugging
} // namespace glrt

#endif // GLRT_DEBUGGING_SHADERDEBUGPRINTER_H
