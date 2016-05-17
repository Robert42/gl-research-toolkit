#ifndef GLRT_SCENE_RESOURCES_STATICMESH_INL
#define GLRT_SCENE_RESOURCES_STATICMESH_INL

#include "static-mesh.h"

namespace glrt {
namespace scene {
namespace resources {


template<int n_Components>
TriangleArray<n_Components> StaticMesh::getTriangleArray() const
{
  TriangleArray<n_Components> result;

  const bool indexed = this->isIndexed();
  const int num_vertices = indexed ? this->indices.length() : this->vertices.length();
  result.resize(num_vertices);

  Q_ASSERT(num_vertices%3 == 0);

  if(this->isIndexed())
  {
#pragma omp parallel for
    for(int i=0; i<num_vertices; i++)
    {
      glm::vec3& vertex = result.vertex(i);
      vertex = this->vertices[this->indices[i]].position;
    }
  }else
  {
#pragma omp parallel for
    for(int i=0; i<num_vertices; i++)
    {
      glm::vec3& vertex = result.vertex(i);
      vertex = this->vertices[i].position;
    }
  }

  return result;
}

template<int n_Components>
void TriangleArray<n_Components>::applyTransformation(const CoordFrame& frame)
{
  const int num_vertices = this->length();

#pragma omp parallel for
  for(int i=0; i<num_vertices; i++)
  {
    glm::vec3& vertex = *reinterpret_cast<glm::vec3*>(i);
    vertex = frame.transform_point(vertex);
  }
}

template<int n_Components>
void TriangleArray<n_Components>::invertNormals()
{
  const int num_vertices = this->length();

  Q_ASSERT(num_vertices%3 == 0);

#pragma omp parallel for
  for(int i=0; i<num_vertices; i+=3)
  {
    glm::vec3& vertexA = this->vertex(i);
    glm::vec3& vertexB = this->vertex(i+2);
    std::swap(vertexA, vertexB);
  }
}

template<int n_Components>
AABB TriangleArray<n_Components>::boundingBox() const
{
  AABB aabb;
  aabb.minPoint = glm::vec3(INFINITY);
  aabb.maxPoint = glm::vec3(-INFINITY);

  const int num_vertices = this->length();

  for(int i=0; i<num_vertices; i++)
  {
    const glm::vec3& vertex = this->vertex(i);
    aabb.minPoint = glm::min(aabb.minPoint, vertex);
    aabb.maxPoint = glm::max(aabb.maxPoint, vertex);
  }

  return aabb;
}

template<int n_Components>
void TriangleArray<n_Components>::resize(int l)
{
  return positions.resize(l);
}

template<int n_Components>
int TriangleArray<n_Components>::length() const
{
  return positions.length();
}

template<int n_Components>
template<int other_n_Components>
void TriangleArray<n_Components>::operator=(const TriangleArray<other_n_Components>& other)
{
  this->positions.resize(other.length());
  const int num_vertices = this->length();

  for(int i=0; i<num_vertices; i++)
  {
    this->vertex(i) = other.vertex(i);
  }
}

template<int n_Components>
void TriangleArray<n_Components>::operator+=(const TriangleArray& other)
{
  const int start = this->length();
  const int num_vertices =  other.length();

  this->positions.resize(this->length() + other.length());

  for(int i=0; i<num_vertices; i++)
  {
    this->vertex(start+i) = other.vertex(i);
  }
}

template<int n_Components>
glm::vec3& TriangleArray<n_Components>::vertex(int i)
{
  byte* const buffer = reinterpret_cast<byte*>(positions.data());
  glm::vec3& vertex = *reinterpret_cast<glm::vec3*>(buffer + i*n_Components*sizeof(float));

  return vertex;
}

template<int n_Components>
const glm::vec3& TriangleArray<n_Components>::vertex(int i) const
{
  const byte* const buffer = reinterpret_cast<const byte*>(positions.data());
  const glm::vec3& vertex = *reinterpret_cast<const glm::vec3*>(buffer + i*n_Components*sizeof(float));

  return vertex;
}

template<int n_Components>
glm::vec3& TriangleArray<n_Components>::operator[](int i)
{
  return vertex(i);
}

template<int n_Components>
const glm::vec3& TriangleArray<n_Components>::operator[](int i) const
{
  return vertex(i);
}


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_STATICMESH_INL
