#ifndef GLRT_RENDERER_STATICMESH_H
#define GLRT_RENDERER_STATICMESH_H

#include <glrt/dependencies.h>
#include <glrt/scene/resources/static-mesh-data.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>

struct aiMesh;

namespace glrt {
namespace renderer {

class StaticMeshBuffer final
{
public:
  typedef scene::resources::StaticMeshData::Vertex Vertex;
  typedef scene::resources::StaticMeshData::index_type index_type;

  StaticMeshBuffer(StaticMeshBuffer&& mesh);
  StaticMeshBuffer(gl::Buffer* indexBuffer, gl::Buffer* vertexBuffer, int numberIndices, int numberVertices);
  ~StaticMeshBuffer();
  StaticMeshBuffer& operator=(StaticMeshBuffer&&);

  StaticMeshBuffer() = delete;
  StaticMeshBuffer(const StaticMeshBuffer&) = delete;
  StaticMeshBuffer& operator=(const StaticMeshBuffer&) = delete;

  static bool isValidFileSuffix(const QFileInfo& file);
  static StaticMeshBuffer loadMeshFile(const QString& file);
  static StaticMeshBuffer create(const scene::resources::StaticMeshData& data);
  static StaticMeshBuffer createIndexed(const index_type* indices, int numIndices, const StaticMeshBuffer::Vertex* vertices, int numVertices, bool indexed = true);
  static StaticMeshBuffer createAsArray(const StaticMeshBuffer::Vertex* vertices, int numVertices);

  static gl::VertexArrayObject generateVertexArrayObject();

  void bind(const gl::VertexArrayObject& vertexArrayObject);

  void draw(GLenum mode = GL_TRIANGLES) const;

private:
  gl::Buffer* indexBuffer;
  gl::Buffer* vertexBuffer;
  int numberIndices;
  int numberVertices;

  static StaticMeshBuffer _createAsArray(const index_type* indices, int numIndices, const StaticMeshBuffer::Vertex* vertices, int numVertices);
};




} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESH_H
