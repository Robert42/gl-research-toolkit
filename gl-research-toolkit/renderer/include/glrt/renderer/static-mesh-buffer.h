#ifndef GLRT_RENDERER_STATICMESH_H
#define GLRT_RENDERER_STATICMESH_H

#include <glrt/dependencies.h>
#include <glrt/scene/resources/static-mesh.h>
#include <glrt/renderer/gl/command-list-recorder.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>

struct aiMesh;

namespace glrt {
namespace renderer {

class StaticMeshBuffer final
{
public:
  typedef scene::resources::StaticMesh::Vertex Vertex;
  typedef scene::resources::StaticMesh::index_type index_type;

  StaticMeshBuffer(StaticMeshBuffer&& mesh);
  StaticMeshBuffer(gl::Buffer* indexBuffer, gl::Buffer* vertexBuffer, int numberIndices, int numberVertices);
  ~StaticMeshBuffer();
  StaticMeshBuffer& operator=(StaticMeshBuffer&&);

  StaticMeshBuffer() = delete;
  StaticMeshBuffer(const StaticMeshBuffer&) = delete;
  StaticMeshBuffer& operator=(const StaticMeshBuffer&) = delete;

  static bool isValidFileSuffix(const QFileInfo& file);
  static StaticMeshBuffer loadMeshFile(const QString& file);
  static StaticMeshBuffer create(const scene::resources::StaticMesh& data);
  static StaticMeshBuffer createIndexed(const index_type* indices, int numIndices, const StaticMeshBuffer::Vertex* vertices, int numVertices, bool indexed = true);
  static StaticMeshBuffer createAsArray(const StaticMeshBuffer::Vertex* vertices, int numVertices);

  static gl::VertexArrayObject generateVertexArrayObject();
  static void enableVertexArrays();
  static void disableVertexArrays();

  void bind(const gl::VertexArrayObject& vertexArrayObject);

  void draw(GLenum mode = GL_TRIANGLES) const;

  void recordBind(gl::CommandListRecorder& recorder) const;
  void recordDraw(gl::CommandListRecorder& recorder) const;
  void recordDraw(gl::CommandListRecorder& recorder, int numInstances) const;

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
