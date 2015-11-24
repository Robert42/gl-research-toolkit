#ifndef GLRT_SCENE_STATICMESH_H
#define GLRT_SCENE_STATICMESH_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>

namespace glrt {
namespace scene {

class StaticMesh final
{
public:
  struct Vertex;
  typedef quint16 index_type;

  StaticMesh() = delete;
  StaticMesh(const StaticMesh&) = delete;
  StaticMesh(StaticMesh&& mesh);
  StaticMesh(gl::Buffer&& indexBuffer, gl::Buffer&& vertexBuffer);

  static StaticMesh loadMeshFromFile(const QString& filename);

public:
  gl::Buffer indexBuffer, vertexBuffer;
};


struct StaticMesh::Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec2 uv;
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_STATICMESH_H
