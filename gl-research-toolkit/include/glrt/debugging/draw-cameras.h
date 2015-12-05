#ifndef GLRT_DEBUGGING_DRAWCAMERAS_H
#define GLRT_DEBUGGING_DRAWCAMERAS_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>

#include "debug-mesh.h"

namespace glrt {
namespace scene {

class Scene;

} // namespace scene

namespace debugging {


class DrawCameras final
{
public:
  DrawCameras();
  ~DrawCameras();

  DrawCameras(const DrawCameras&) = delete;
  DrawCameras(DrawCameras&&) = delete;
  DrawCameras& operator=(const DrawCameras&) = delete;
  DrawCameras& operator=(DrawCameras&&) = delete;

  void draw(const scene::Scene* scene);

  void deinit();

private:
  gl::Buffer* cameraParameter = nullptr;
  DebugMesh* debugMesh = nullptr;
  int nCameras = 0;
};


} // namespace debugging
} // namespace glrt

#endif // GLRT_DEBUGGING_DRAWCAMERAS_H
