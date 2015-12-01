#ifndef GLRT_SCENE_RENDERER_H
#define GLRT_SCENE_RENDERER_H

#include "scene.h"

namespace glrt {
namespace scene {

class Renderer final
{
public:
  Scene& scene;

  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  Renderer(Scene& scene);
  ~Renderer();

  void cacheScene();
  void renderScene();

private:
  quint64 _cachedStaticStructureCacheIndex = -1;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RENDERER_H
