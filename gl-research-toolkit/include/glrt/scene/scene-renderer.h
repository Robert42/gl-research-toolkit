#ifndef GLRT_SCENE_RENDERER_H
#define GLRT_SCENE_RENDERER_H

#include "scene.h"

namespace glrt {
namespace scene {

class Renderer
{
public:
  class Pass;
  class Pass;

  Scene& scene;

  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  Renderer(Scene& scene);
  virtual ~Renderer();


protected:
  void updateCache();


private:
  quint64 _cachedStaticStructureCacheIndex = 0;
};


class Renderer::Pass final
{
public:
  gl::ShaderObject shaderObject;

  Pass(gl::ShaderObject&& shaderObject);
  Pass(const QString& materialName, const QStringList& preprocessorMacros);
};



} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RENDERER_H
