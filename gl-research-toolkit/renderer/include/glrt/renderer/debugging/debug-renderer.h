#ifndef GLRT_RENDERER_DEBUGGING_DEBUGRENDERER_H
#define GLRT_RENDERER_DEBUGGING_DEBUGRENDERER_H

#include <glrt/gui/anttweakbar.h>

namespace glrt {
namespace renderer {
namespace debugging {

class DebugRenderer
{
public:
  gui::TweakBarCBVar<bool> guiToggle;

  DebugRenderer(scene::Scene* scene);
  DebugRenderer(const DebugRenderer&);
  DebugRenderer(DebugRenderer&& other);
  ~DebugRenderer();

  DebugRenderer& operator=(const DebugRenderer&) = delete;
  DebugRenderer& operator=(DebugRenderer&&) = delete;

  virtual void reinit() = 0;
  virtual void render() = 0;

  void setEnabled(bool enabled);
  bool isEnabled() const;

  scene::Scene* scene() const;

private:
  scene::Scene* _scene;
  QMetaObject::Connection loadSceneConnection;

  bool _enabled = false;
};

} // namespace debugging
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEBUGGING_DEBUGRENDERER_H
