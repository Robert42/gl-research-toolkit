#include <glrt/scene/resources/static-mesh-loader.h>
#include <glrt/renderer/static-mesh-buffer.h>
#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace renderer {

const GLuint vertexBufferBinding = 0;
const GLuint indexBufferBinding = 1;

StaticMeshBuffer::StaticMeshBuffer(gl::Buffer* indexBuffer, gl::Buffer* vertexBuffer, int numberIndices, int numberVertices, const AABB& aabb)
  : indexBuffer(std::move(indexBuffer)),
    vertexBuffer(std::move(vertexBuffer)),
    numberIndices(numberIndices),
    numberVertices(numberVertices),
    _aabb(aabb)
{
}

StaticMeshBuffer::StaticMeshBuffer(StaticMeshBuffer&& mesh)
  : indexBuffer(std::move(mesh.indexBuffer)),
    vertexBuffer(std::move(mesh.vertexBuffer)),
    numberIndices(mesh.numberIndices),
    numberVertices(mesh.numberVertices),
    _aabb(mesh._aabb)
{
  mesh.numberIndices = 0;
  mesh.numberVertices = 0;
  mesh.indexBuffer = nullptr;
  mesh.vertexBuffer = nullptr;
  mesh._aabb = AABB::invalid();
}


StaticMeshBuffer::~StaticMeshBuffer()
{
  delete indexBuffer;
  delete vertexBuffer;
}


StaticMeshBuffer& StaticMeshBuffer::operator=(StaticMeshBuffer&& mesh)
{
  std::swap(mesh.indexBuffer, this->indexBuffer);
  std::swap(mesh.vertexBuffer, this->vertexBuffer);
  std::swap(mesh.numberIndices, this->numberIndices);
  std::swap(mesh.numberVertices, this->numberVertices);
  return *this;
}


bool StaticMeshBuffer::isValidFileSuffix(const QFileInfo& file)
{
  return file.suffix().toLower() == "mesh";
}


StaticMeshBuffer StaticMeshBuffer::loadMeshFile(const QString& file)
{
  return create(scene::resources::StaticMesh::loadFromFile(file));
}


StaticMeshBuffer StaticMeshBuffer::create(const scene::resources::StaticMesh& data)
{
  if(data.isIndexed())
    return createIndexed(data.indices.data(), data.indices.length(), data.vertices.data(), data.vertices.length());
  else
    return createAsArray(data.vertices.data(), data.vertices.length());
}


StaticMeshBuffer StaticMeshBuffer::createIndexed(const index_type* indices, int numIndices, const StaticMeshBuffer::Vertex* vertices, int numVertices, bool indexed)
{
  if(!indexed)
    return _createAsArray(indices, numIndices, vertices, numVertices);

  gl::Buffer* indexBuffer = new gl::Buffer(numIndices*sizeof(index_type), gl::Buffer::UsageFlag::IMMUTABLE, indices);
  gl::Buffer* vertexBuffer = new gl::Buffer(numVertices*sizeof(Vertex), gl::Buffer::UsageFlag::IMMUTABLE, vertices);

  return StaticMeshBuffer(indexBuffer,
                          vertexBuffer,
                          numIndices,
                          numVertices,
                          AABB::fromVertices(&vertices->position, numVertices, sizeof(Vertex)));
}


StaticMeshBuffer StaticMeshBuffer::_createAsArray(const index_type* indices, int numIndices, const StaticMeshBuffer::Vertex* vertices, int numVertices)
{
  std::vector<StaticMeshBuffer::Vertex> newVertices;

  newVertices.reserve(numIndices);

  for(int i=0; i<numIndices; ++i)
  {
    Q_ASSERT(indices[i] < quint32(numVertices));

    newVertices.push_back(vertices[indices[i]]);
  }

  Q_UNUSED(numVertices);

  return createAsArray(newVertices.data(), newVertices.size());
}


StaticMeshBuffer StaticMeshBuffer::createAsArray(const StaticMeshBuffer::Vertex* vertices, int numVertices)
{
  gl::Buffer* indexBuffer = nullptr;
  int numIndices = 0;

  gl::Buffer* vertexBuffer = new gl::Buffer(numVertices*sizeof(Vertex), gl::Buffer::UsageFlag::IMMUTABLE, vertices);

  return StaticMeshBuffer(indexBuffer,
                          vertexBuffer,
                          numIndices,
                          numVertices,
                          AABB::fromVertices(&vertices->position, numVertices, sizeof(Vertex)));
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
gl::VertexArrayObject StaticMeshBuffer::generateVertexArrayObject()
{
  typedef gl::VertexArrayObject::Attribute Attribute;

  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_POSITION == 0);
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_NORMAL == 1);
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_TANGENT == 2);
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_BITANGENT == 3);
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_UV == 4);

  return std::move(gl::VertexArrayObject({Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 2, vertexBufferBinding)}));
}


void StaticMeshBuffer::enableVertexArrays()
{
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_POSITION == 0);
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_NORMAL == 1);
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_TANGENT == 2);
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_BITANGENT == 3);
  Q_ASSERT(VERTEX_ATTRIBUTE_LOCATION_UV == 4);

  GL_CALL(glVertexAttribFormatNV, VERTEX_ATTRIBUTE_LOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex));
  GL_CALL(glVertexAttribFormatNV, VERTEX_ATTRIBUTE_LOCATION_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex));
  GL_CALL(glVertexAttribFormatNV, VERTEX_ATTRIBUTE_LOCATION_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex));
  GL_CALL(glVertexAttribFormatNV, VERTEX_ATTRIBUTE_LOCATION_BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex));
  GL_CALL(glVertexAttribFormatNV, VERTEX_ATTRIBUTE_LOCATION_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex));

  GL_CALL(glEnableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_POSITION);
  GL_CALL(glEnableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_NORMAL);
  GL_CALL(glEnableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_TANGENT);
  GL_CALL(glEnableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_BITANGENT);
  GL_CALL(glEnableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_UV);
}

void StaticMeshBuffer::disableVertexArrays()
{
  GL_CALL(glDisableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_POSITION);
  GL_CALL(glDisableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_NORMAL);
  GL_CALL(glDisableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_TANGENT);
  GL_CALL(glDisableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_BITANGENT);
  GL_CALL(glDisableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_UV);
}


void StaticMeshBuffer::bind(const gl::VertexArrayObject& vertexArrayObject)
{
  if(indexBuffer != nullptr)
    indexBuffer->BindIndexBuffer();
  vertexBuffer->BindVertexBuffer(vertexBufferBinding, 0, vertexArrayObject.GetVertexStride(vertexBufferBinding));
}

void StaticMeshBuffer::draw(GLenum mode) const
{
  GLenum gl_index_type;
  if(sizeof(index_type) == 2)
    gl_index_type = GL_UNSIGNED_SHORT;
  else if(sizeof(index_type) == 4)
    gl_index_type = GL_UNSIGNED_INT;
  else
    Q_UNREACHABLE();

  if(indexBuffer != nullptr)
    GL_CALL(glDrawElements, mode, numberIndices, gl_index_type, nullptr);
  else
    GL_CALL(glDrawArrays, mode, 0, numberVertices);
}

void StaticMeshBuffer::recordDrawInstances(gl::CommandListRecorder& recorder, int begin, int end) const
{
  int numInstances = end-begin;

  if(indexBuffer != nullptr)
  {
    recorder.append_token_DrawElementsInstanced(GL_TRIANGLES, numberIndices, numInstances, 0, 0, begin);
  }else
  {
    recorder.append_token_DrawArraysInstanced(GL_TRIANGLES, numberIndices, numInstances, 0, begin);
  }
}

void StaticMeshBuffer::recordDraw(gl::CommandListRecorder& recorder) const
{
  if(indexBuffer != nullptr)
    recorder.append_token_DrawElements(numberIndices, 0, 0, gl::CommandListRecorder::Strip::NO_STRIP);
  else
    recorder.append_token_DrawArrays(numberVertices, 0, gl::CommandListRecorder::Strip::NO_STRIP);
}

void StaticMeshBuffer::recordBind(gl::CommandListRecorder& recorder) const
{
  GLuint64 offset = 0;
  recorder.append_token_AttributeAddress(VERTEX_ATTRIBUTE_LOCATION_POSITION, vertexBuffer->gpuBufferAddress()+offset);
  offset += 3 * sizeof(float);
  recorder.append_token_AttributeAddress(VERTEX_ATTRIBUTE_LOCATION_NORMAL, vertexBuffer->gpuBufferAddress()+offset);
  offset += 3 * sizeof(float);
  recorder.append_token_AttributeAddress(VERTEX_ATTRIBUTE_LOCATION_TANGENT, vertexBuffer->gpuBufferAddress()+offset);
  offset += 3 * sizeof(float);
  recorder.append_token_AttributeAddress(VERTEX_ATTRIBUTE_LOCATION_BITANGENT, vertexBuffer->gpuBufferAddress()+offset);
  offset += 3 * sizeof(float);
  recorder.append_token_AttributeAddress(VERTEX_ATTRIBUTE_LOCATION_UV, vertexBuffer->gpuBufferAddress()+offset);
  offset += 2 * sizeof(float);

  if(indexBuffer != nullptr)
  {
    recorder.append_token_ElementAddress(indexBuffer->gpuBufferAddress(), sizeof(index_type));
  }
}


} // namespace renderer
} // namespace glrt

