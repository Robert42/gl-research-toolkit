#ifndef GLRT_RENDERER_DEBUGGING_DEBUGGINGPOSTEFFECT_H
#define GLRT_RENDERER_DEBUGGING_DEBUGGINGPOSTEFFECT_H

#include <glrt/renderer/debugging/debug-renderer.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>

namespace glrt {
namespace renderer {
namespace debugging {

class DebuggingPosteffect
{
public:
  DebuggingPosteffect() = delete;

  static DebugRenderer orangeScreen(float zValue, float radius, bool depthTest);
  static DebugRenderer overlayLightSources();
};

} // namespace debugging
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEBUGGING_DEBUGGINGPOSTEFFECT_H
