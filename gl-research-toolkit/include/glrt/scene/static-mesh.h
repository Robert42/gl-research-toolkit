#ifndef GLRT_SCENE_STATICMESH_H
#define GLRT_SCENE_STATICMESH_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>

namespace glrt {
namespace scene {

class StaticMesh final
{
public:
  StaticMesh() = delete;
  StaticMesh(gl::Buffer&& buffer);

  static StaticMesh&& loadMeshFromFile(const QString& filename);

public:
  gl::Buffer buffer;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_STATICMESH_H
