#include <glrt/scene/aabb.h>

namespace glrt {
namespace scene {

AABB AABB::fromVertices(const glm::vec3* vertices, int numVertices)
{
  return fromVertices(vertices, numVertices, sizeof(glm::vec3));
}

AABB AABB::fromVertices(const glm::vec3* vertices, int numVertices, size_t stride)
{
  AABB aabb;
  aabb.minPoint = glm::vec3(INFINITY);
  aabb.maxPoint = glm::vec3(-INFINITY);
  size_t address = size_t(vertices);

  for(int i=0; i<numVertices; ++i)
  {
    const glm::vec3& v = *reinterpret_cast<const glm::vec3*>(address);

    aabb.minPoint = glm::min(v, aabb.minPoint);
    aabb.maxPoint = glm::max(v, aabb.maxPoint);

    address += stride;
  }

  return aabb;
}

AABB AABB::aabbOfTransformedBoundingBox(const CoordFrame& coordFrame) const
{
  glm::vec3 p[2] = {this->minPoint, this->maxPoint};

  AABB aabb;
  aabb.minPoint = glm::vec3(INFINITY);
  aabb.maxPoint = glm::vec3(-INFINITY);
  for(int i=0; i<8; ++i)
  {
    glm::ivec3 p_index((i&4) > 0,
                       (i&2) > 0,
                       (i&1) > 0);

    glm::vec3 v = coordFrame.transform_point(glm::vec3(p[p_index.x].x,
                                                       p[p_index.y].y,
                                                       p[p_index.z].z));

    aabb.minPoint = glm::min(v, aabb.minPoint);
    aabb.maxPoint = glm::max(v, aabb.maxPoint);
  }

  return aabb;
}


} // namespace scene
} // namespace glrt
