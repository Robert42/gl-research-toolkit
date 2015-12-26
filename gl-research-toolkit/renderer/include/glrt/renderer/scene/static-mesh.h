#ifndef GLRT_SCENE_STATICMESH_H
#define GLRT_SCENE_STATICMESH_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>

struct aiMesh;

namespace glrt {
namespace scene {

class StaticMesh final
{
public:
  struct Vertex;
  typedef quint16 index_type;

  StaticMesh(StaticMesh&& mesh);
  StaticMesh(gl::Buffer* indexBuffer, gl::Buffer* vertexBuffer, int numberIndices, int numberVertices);
  ~StaticMesh();
  StaticMesh& operator=(StaticMesh&&);

  StaticMesh() = delete;
  StaticMesh(const StaticMesh&) = delete;
  StaticMesh& operator=(const StaticMesh&) = delete;

  static bool isValidFile(const QFileInfo& file, bool parseFile);
  static StaticMesh loadMeshFromFile(const QString& file, bool indexed=true);
  static StaticMesh loadMeshFromAssimp(aiMesh** meshes, quint32 nMeshes, const glm::mat3& transformation, const QString& context, bool indexed);
  static StaticMesh createIndexed(const index_type* indices, int numIndices, const StaticMesh::Vertex* vertices, int numVertices, bool indexed = true);
  static StaticMesh createAsArray(const StaticMesh::Vertex* vertices, int numVertices);

  static gl::VertexArrayObject generateVertexArrayObject();

  void bind(const gl::VertexArrayObject& vertexArrayObject);

  void draw(GLenum mode = GL_TRIANGLES) const;

private:
  gl::Buffer* indexBuffer;
  gl::Buffer* vertexBuffer;
  int numberIndices;
  int numberVertices;

  static StaticMesh _createAsArray(const index_type* indices, int numIndices, const StaticMesh::Vertex* vertices, int numVertices);
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
