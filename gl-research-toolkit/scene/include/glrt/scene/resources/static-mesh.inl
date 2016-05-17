#ifndef GLRT_SCENE_RESOURCES_STATICMESH_INL
#define GLRT_SCENE_RESOURCES_STATICMESH_INL

#include "static-mesh.h"

namespace glrt {
namespace scene {
namespace resources {


inline TriangleArray StaticMesh::getTriangleArray() const
{
  TriangleArray result;

  const bool indexed = this->isIndexed();
  const int num_vertices = indexed ? this->indices.length() : this->vertices.length();
  result.vertices.resize(num_vertices);

  Q_ASSERT(num_vertices%3 == 0);

  if(this->isIndexed())
  {
#pragma omp parallel for
    for(int i=0; i<num_vertices; i++)
    {
      glm::vec3& vertex = result.vertices[i];
      vertex = this->vertices[this->indices[i]].position;
    }
  }else
  {
#pragma omp parallel for
    for(int i=0; i<num_vertices; i++)
    {
      glm::vec3& vertex = result.vertices[i];
      vertex = this->vertices[i].position;
    }
  }

  return result;
}

inline void TriangleArray::applyTransformation(const CoordFrame& frame)
{
  const int num_vertices = this->vertices.length();

#pragma omp parallel for
  for(int i=0; i<num_vertices; i++)
  {
    glm::vec3& vertex = this->vertices[i];
    vertex = frame.transform_point(vertex);
  }
}

inline void TriangleArray::invertNormals()
{
  const int num_vertices = this->vertices.length();

  Q_ASSERT(num_vertices%3 == 0);

#pragma omp parallel for
  for(int i=0; i<num_vertices; i+=3)
  {
    glm::vec3& vertexA = this->vertices[i];
    glm::vec3& vertexB = this->vertices[i+2];
    std::swap(vertexA, vertexB);
  }
}

inline AABB TriangleArray::boundingBox() const
{
  AABB aabb;
  aabb.minPoint = glm::vec3(INFINITY);
  aabb.maxPoint = glm::vec3(-INFINITY);

  const int num_vertices = this->vertices.length();

  for(int i=0; i<num_vertices; i++)
  {
    const glm::vec3& vertex = this->vertices[i];
    aabb.minPoint = glm::min(aabb.minPoint, vertex);
    aabb.maxPoint = glm::max(aabb.maxPoint, vertex);
  }

  return aabb;
}

inline glm::vec3& TriangleArray::operator[](int i)
{
  return vertices[i];
}

inline const glm::vec3& TriangleArray::operator[](int i) const
{
  return vertices[i];
}


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_STATICMESH_INL
