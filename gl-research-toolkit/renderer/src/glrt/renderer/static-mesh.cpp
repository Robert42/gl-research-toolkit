#include <glrt/scene/resources/resource-loader.h>
#include <glrt/renderer/static-mesh.h>
#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace renderer {

const GLuint vertexBufferBinding = 0;
const GLuint indexBufferBinding = 1;

StaticMesh::StaticMesh(gl::Buffer* indexBuffer, gl::Buffer* vertexBuffer, int numberIndices, int numberVertices)
  : indexBuffer(std::move(indexBuffer)),
    vertexBuffer(std::move(vertexBuffer)),
    numberIndices(numberIndices),
    numberVertices(numberVertices)
{
}

StaticMesh::StaticMesh(StaticMesh&& mesh)
  : indexBuffer(std::move(mesh.indexBuffer)),
    vertexBuffer(std::move(mesh.vertexBuffer)),
    numberIndices(mesh.numberIndices),
    numberVertices(mesh.numberVertices)
{
  mesh.numberIndices = 0;
  mesh.numberVertices = 0;
  mesh.indexBuffer = nullptr;
  mesh.vertexBuffer = nullptr;
}


StaticMesh::~StaticMesh()
{
  delete indexBuffer;
  delete vertexBuffer;
}


StaticMesh& StaticMesh::operator=(StaticMesh&& mesh)
{
  std::swap(mesh.indexBuffer, this->indexBuffer);
  std::swap(mesh.vertexBuffer, this->vertexBuffer);
  std::swap(mesh.numberIndices, this->numberIndices);
  std::swap(mesh.numberVertices, this->numberVertices);
  return *this;
}


bool StaticMesh::isValidFileSuffix(const QFileInfo& file)
{
  return file.suffix().toLower() == "mesh";
}


StaticMesh StaticMesh::loadMeshFile(const QString& file)
{
  return create(scene::resources::StaticMeshData::loadFromFile(file));
}


StaticMesh StaticMesh::create(const scene::resources::StaticMeshData& data)
{
  if(data.isIndexed())
    return createIndexed(data.indices.data(), data.indices.length(), data.vertices.data(), data.vertices.length());
  else
    return createAsArray(data.vertices.data(), data.vertices.length());
}


StaticMesh StaticMesh::createIndexed(const index_type* indices, int numIndices, const StaticMesh::Vertex* vertices, int numVertices, bool indexed)
{
  if(!indexed)
    return _createAsArray(indices, numIndices, vertices, numVertices);

  gl::Buffer* indexBuffer = new gl::Buffer(numIndices*sizeof(index_type), gl::Buffer::UsageFlag::IMMUTABLE, indices);
  gl::Buffer* vertexBuffer = new gl::Buffer(numVertices*sizeof(Vertex), gl::Buffer::UsageFlag::IMMUTABLE, vertices);

  return StaticMesh(indexBuffer,
                    vertexBuffer,
                    numIndices,
                    numVertices);
}


StaticMesh StaticMesh::_createAsArray(const index_type* indices, int numIndices, const StaticMesh::Vertex* vertices, int numVertices)
{
  std::vector<StaticMesh::Vertex> newVertices;

  newVertices.reserve(numIndices);

  for(int i=0; i<numIndices; ++i)
  {
    Q_ASSERT(indices[i] < numVertices);

    newVertices.push_back(vertices[indices[i]]);
  }

  Q_UNUSED(numVertices);

  return createAsArray(newVertices.data(), newVertices.size());
}


StaticMesh StaticMesh::createAsArray(const StaticMesh::Vertex* vertices, int numVertices)
{
  gl::Buffer* indexBuffer = nullptr;
  int numIndices = 0;

  gl::Buffer* vertexBuffer = new gl::Buffer(numVertices*sizeof(Vertex), gl::Buffer::UsageFlag::IMMUTABLE, vertices);

  return StaticMesh(indexBuffer,
                    vertexBuffer,
                    numIndices,
                    numVertices);
}


// Normally, only one instance of gl::VertexArrayObject should be needed, as it only defined the format, which is reusable.
// Quote (https://www.opengl.org/wiki/Vertex_Specification#Separate_attribute_format):
// Better yet, if you want to use the same format but move the buffer around, it only takes one function call; namely glBindVertexBuffer​ with a buffer binding of 0.
//
//
// in other words, you can do the following:
// 1. Bind vertex array object
// 2. Bind Buffer to 0
// 3. Draw
// 4. Bind other Buffer to 0
// 5. Draw
// 6. Bind other Buffer to 0
// 7. Draw
gl::VertexArrayObject StaticMesh::generateVertexArrayObject()
{
  typedef gl::VertexArrayObject::Attribute Attribute;

  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_POSITION == 0);
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_NORMAL == 1);
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_TANGENT == 2);
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_UV == 3);

  return std::move(gl::VertexArrayObject({Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 2, vertexBufferBinding)}));
}

void StaticMesh::bind(const gl::VertexArrayObject& vertexArrayObject)
{
  if(indexBuffer != nullptr)
    indexBuffer->BindIndexBuffer();
  vertexBuffer->BindVertexBuffer(vertexBufferBinding, 0, vertexArrayObject.GetVertexStride(vertexBufferBinding));
}

void StaticMesh::draw(GLenum mode) const
{
  Q_ASSERT(sizeof(index_type) == 2); // assert, that GL_UNSIGNED_SHORT is the right type
  if(indexBuffer != nullptr)
    GL_CALL(glDrawElements, mode, numberIndices, GL_UNSIGNED_SHORT, nullptr);
  else
    GL_CALL(glDrawArrays, mode, 0, numberVertices);
}


} // namespace renderer
} // namespace glrt

