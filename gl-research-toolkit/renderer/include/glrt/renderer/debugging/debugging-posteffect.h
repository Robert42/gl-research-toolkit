#ifndef GLRT_RENDERER_DEBUGGING_DEBUGGINGPOSTEFFECT_H
#define GLRT_RENDERER_DEBUGGING_DEBUGGINGPOSTEFFECT_H

#include <glrt/renderer/debugging/debug-renderer.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>

namespace glrt {
namespace renderer {
namespace debugging {

class DebuggingPosteffect : public DebugRenderer
{
public:
  static DebuggingPosteffect orangeScreen(float zValue, float radius, bool depthTest);

  DebuggingPosteffect(scene::Scene* scene, ReloadableShader&& shader, bool depthTest);
  DebuggingPosteffect(DebuggingPosteffect&& other);
  ~DebuggingPosteffect();

  DebuggingPosteffect(const DebuggingPosteffect& other) = delete;
  DebuggingPosteffect& operator=(const DebuggingPosteffect&) = delete;
  DebuggingPosteffect& operator=(DebuggingPosteffect&&) = delete;

  void reinit() override;
  void render() override;

private:
  scene::Scene* _scene;
  ReloadableShader _shader;
  bool _depthTest;
};

} // namespace debugging
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEBUGGING_DEBUGGINGPOSTEFFECT_H
