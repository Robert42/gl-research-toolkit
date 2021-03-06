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
  result.vertices.resize(size_t(num_vertices));

  Q_ASSERT(num_vertices%3 == 0);

  if(this->isIndexed())
  {
#pragma omp parallel for
    for(int i=0; i<num_vertices; i++)
    {
      glm::vec3& vertex = result.vertices[size_t(i)];
      vertex = this->vertices[this->indices[i]].position;
    }
  }else
  {
#pragma omp parallel for
    for(int i=0; i<num_vertices; i++)
    {
      glm::vec3& vertex = result.vertices[size_t(i)];
      vertex = this->vertices[i].position;
    }
  }

  return result;
}

inline void TriangleArray::applyTransformation(const CoordFrame& frame)
{
  const size_t num_vertices = this->vertices.size();

#pragma omp parallel for
  for(size_t i=0; i<num_vertices; i++)
  {
    glm::vec3& vertex = this->vertices[i];
    vertex = frame.transform_point(vertex);
  }
}

inline void TriangleArray::invertNormals()
{
  const size_t num_vertices = this->vertices.size();

  Q_ASSERT(num_vertices%3 == 0);

#pragma omp parallel for
  for(size_t i=0; i<num_vertices; i+=3)
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

  const size_t num_vertices = this->vertices.size();

  for(size_t i=0; i<num_vertices; i++)
  {
    const glm::vec3& vertex = this->vertices[i];
    aabb.minPoint = glm::min(aabb.minPoint, vertex);
    aabb.maxPoint = glm::max(aabb.maxPoint, vertex);
  }

  return aabb;
}

inline BoundingSphere TriangleArray::boundingSphere() const
{
  AABB aabb;
  BoundingSphere boundingSphere;

  boundingShapes(boundingSphere, aabb);

  return boundingSphere;
}

inline void TriangleArray::boundingShapes(BoundingSphere& sphere, AABB& aabb) const
{
  aabb = boundingBox();
  sphere = BoundingSphere{(aabb.maxPoint+aabb.minPoint)*0.5f, 0};

  const size_t num_vertices = this->vertices.size();

  for(size_t i=0; i<num_vertices; i++)
  {
    const glm::vec3& vertex = this->vertices[i];
    sphere.radius = glm::max(sphere.radius, distance(vertex, sphere.center));
  }
}

inline void TriangleArray::operator+=(const TriangleArray& other)
{
  this->vertices.insert(this->vertices.end(), other.vertices.begin(), other.vertices.end());
}

inline glm::vec3& TriangleArray::operator[](size_t i)
{
  return vertices[i];
}

inline const glm::vec3& TriangleArray::operator[](size_t i) const
{
  return vertices[i];
}

inline BoundingSphere BoundingSphere::operator|(const BoundingSphere& b) const
{
  const BoundingSphere& a = *this;

  glm::vec3 a2b = b.center - a.center;
  float distance = glm::length(a2b);

  float e1 = glm::min(-a.radius, distance-b.radius);
  float e2 = glm::max( a.radius, distance+b.radius);

  glm::vec3 a2b_dir = a2b / distance;

  glm::bvec3 replace_with_zero = glm::isnan(a2b_dir) || glm::isinf(a2b_dir);
  for(int i=0; i<3; ++i)
    if(replace_with_zero[i])
      a2b_dir[i] = 0;

  glm::vec3 edge1 = a.center + a2b_dir * e1;
  glm::vec3 edge2 = a.center + a2b_dir * e2;

  BoundingSphere joined;

  joined.center = glm::mix(edge1, edge2, 0.5f);
  joined.radius = glm::max(joined.radius_for_enclosing_contained_spehre(a), joined.radius_for_enclosing_contained_spehre(b)) + 1.e-7f;

  Q_ASSERT(joined.contains(a, 0.f));
  Q_ASSERT(joined.contains(b, 0.f));

  return joined;
}

inline bool BoundingSphere::contains(const glm::vec3& point, float epsilon) const
{
  return glm::distance(point, this->center) <= this->radius+epsilon;
}

inline bool BoundingSphere::contains(const BoundingSphere& b, float epsilon) const
{
  if(b.center==this->center)
    return b.radius <= this->radius+epsilon;
  else
    return radius_for_enclosing_contained_spehre(b) <= this->radius+epsilon;
}

inline float BoundingSphere::radius_for_enclosing_contained_spehre(const BoundingSphere& other) const
{
  return glm::distance(other.center, this->center) + other.radius;
}


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_STATICMESH_INL
