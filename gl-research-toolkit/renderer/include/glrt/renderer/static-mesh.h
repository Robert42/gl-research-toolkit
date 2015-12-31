#ifndef GLRT_RENDERER_STATICMESH_H
#define GLRT_RENDERER_STATICMESH_H

#include <glrt/dependencies.h>
#include <glrt/scene/resources/static-mesh-data.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>

struct aiMesh;

namespace glrt {
namespace renderer {

class StaticMesh final
{
public:
  typedef scene::resources::StaticMeshData::Vertex Vertex;
  typedef scene::resources::StaticMeshData::index_type index_type;

  StaticMesh(StaticMesh&& mesh);
  StaticMesh(gl::Buffer* indexBuffer, gl::Buffer* vertexBuffer, int numberIndices, int numberVertices);
  ~StaticMesh();
  StaticMesh& operator=(StaticMesh&&);

  StaticMesh() = delete;
  StaticMesh(const StaticMesh&) = delete;
  StaticMesh& operator=(const StaticMesh&) = delete;

  static bool isValidFileSuffix(const QFileInfo& file);
  static StaticMesh loadMeshFile(const QString& file);
  static StaticMesh create(const scene::resources::StaticMeshData& data);
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




} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESH_H
