#include <glrt/debugging/debug-mesh.h>
#include <glrt/toolkit/geometry.h>
#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace debugging {


DebugMesh::DebugMesh(const Vertex* vertices, int numVertices)
  : vertexBuffer(sizeof(Vertex)*numVertices, gl::Buffer::UsageFlag::IMMUTABLE, vertices),
    numVertices(numVertices)
{
}

DebugMesh::~DebugMesh()
{
}


DebugMesh::DebugMesh(DebugMesh&& debugMesh)
  : vertexBuffer(std::move(debugMesh.vertexBuffer)),
    numVertices(debugMesh.numVertices)
{
  debugMesh.numVertices = 0;
}


gl::VertexArrayObject DebugMesh::generateVertexArrayObject()
{
  typedef gl::VertexArrayObject::Attribute Attribute;

  Q_ASSERT(DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_POSITION == 0);
  Q_ASSERT(DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER1 == 1);
  Q_ASSERT(DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_COLOR == 2);
  Q_ASSERT(DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER2 == 3);

  const int vertexBufferBinding = 0;

  return std::move(gl::VertexArrayObject({Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 1, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 1, vertexBufferBinding)}));
}


void DebugMesh::bind(const gl::VertexArrayObject& vertexArrayObject)
{
  const int vertexBufferBinding = 0;
  vertexBuffer.BindVertexBuffer(vertexBufferBinding, 0, vertexArrayObject.GetVertexStride(vertexBufferBinding));
}


void DebugMesh::draw()
{
  GL_CALL(glDrawArrays, GL_LINES, 0, numVertices);
}


// ======== Painter ============================================================


DebugMesh::Painter::Painter()
{
  transformations.push(glm::mat4(1));
}


void DebugMesh::Painter::beginStrip(bool close, int modulo)
{
  this->stripIndex = 0;
  this->stripModulo = modulo;
  if(close)
    this->firstStripVertex = vertices.length();
  else
    this->firstStripVertex = std::numeric_limits<int>::max();
}

void DebugMesh::Painter::endStrip()
{
  if(this->firstStripVertex < vertices.length())
    addVertex(vertices[this->firstStripVertex].position);

  this->stripIndex = -1;
}


void DebugMesh::Painter::addVertex(const glm::vec3& position)
{
  Q_ASSERT(transformations.size() > 0);

  if(stripIndex >= 0)
  {
    if(vertices.length() > 0 && stripIndex > 0)
    {
      Vertex v = vertices.last();
      vertices.append(v);
    }
    stripIndex  = stripIndex % stripModulo;;
  }

  vertices.resize(vertices.length()+1);

  glm::vec4 transformed = transformations.top() * glm::vec4(position, 1);

  vertices.last().position = transformed.xyz() / transformed.w;
  vertices.last().color = nextAttribute.color;
  vertices.last().parameter1 = nextAttribute.parameter1;
  vertices.last().parameter2 = nextAttribute.parameter2;
}

void DebugMesh::Painter::addVertex(const glm::vec2& position, float z)
{
  addVertex(glm::vec3(position, z));
}

void DebugMesh::Painter::addVertex(float x, float y, float z)
{
  addVertex(glm::vec3(x, y, z));
}


void DebugMesh::Painter::addCircle(float radius, int nPoints)
{
  beginStrip(true);
  for(int i=0; i<=nPoints; ++i)
  {
    float angle = i * glm::two_pi<float>() / nPoints;

    addVertex(glm::vec2(glm::cos(angle), glm::sin(angle)) * radius);
  }
  endStrip();
}


void DebugMesh::Painter::addSphere(float radius, int nPoints)
{
  addCircle(radius, nPoints);

  pushMatrix(glm::vec3(0), glm::vec3(1, 0, 0));
  addCircle(radius, nPoints);
  popMatrix();

  pushMatrix(glm::vec3(0), glm::vec3(0, 1, 0));
  addCircle(radius, nPoints);
  popMatrix();
}


void DebugMesh::Painter::pushMatrix(const glm::vec3& position, const glm::vec3& normal, bool multiply)
{
  pushMatrix(position, normal, find_best_perpendicular(normal), multiply);
}

void DebugMesh::Painter::pushMatrix(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& firstPointDirection, bool multiply)
{
  glm::mat4 matrix = glm::mat4(glm::vec4(firstPointDirection, 0),
                               glm::vec4(glm::cross(normal, firstPointDirection), 0),
                               glm::vec4(normal, 0),
                               glm::vec4(position, 1));

  pushMatrix(matrix, multiply);
}

void DebugMesh::Painter::pushMatrix(const glm::mat4& matrix, bool multiply)
{
  Q_ASSERT(transformations.size() > 0);

  if(multiply)
    transformations.push(transformations.top() * matrix);
  else
    transformations.push(matrix);
}

void DebugMesh::Painter::popMatrix()
{
  Q_ASSERT(transformations.size() > 0);
  transformations.pop();

  if(transformations.size() == 0)
    transformations.push(glm::mat4(1));
}


DebugMesh DebugMesh::Painter::toMesh() const
{
  return DebugMesh(vertices.data(), vertices.length());
}


} // namespace debugging
} // namespace glrt

