#include <glrt/scene/aabb.h>

namespace glrt {
namespace scene {

AABB AABB::fromVertices(const glm::vec3* vertices, int numVertices)
{
  return fromVertices(vertices, numVertices, sizeof(glm::vec3));
}

AABB AABB::invalid()
{
  AABB aabb;
  aabb.minPoint = glm::vec3(INFINITY);
  aabb.maxPoint = glm::vec3(-INFINITY);

  return aabb;
}

glm::vec3 AABB::toUnitSpace(const glm::vec3& v) const
{
  return (v-this->minPoint) / (this->maxPoint-this->minPoint);
}

glm::vec3 AABB::size() const
{
  return maxPoint-minPoint;
}

void AABB::operator |= (const AABB& other)
{
  Q_ASSERT(other.isValid());
  *this |= other.maxPoint;
  *this |= other.minPoint;
}

void AABB::operator |= (const glm::vec3& other)
{
  this->minPoint = glm::min(other, this->minPoint);
  this->maxPoint = glm::max(other, this->maxPoint);
}

AABB AABB::fromVertices(const glm::vec3* vertices, int numVertices, size_t stride)
{
  AABB aabb = AABB::invalid();
  size_t address = size_t(vertices);

  for(int i=0; i<numVertices; ++i)
  {
    const glm::vec3& v = *reinterpret_cast<const glm::vec3*>(address);

    aabb |= v;

    address += stride;
  }

  return aabb;
}

AABB AABB::aabbOfTransformedBoundingBox(const CoordFrame& coordFrame) const
{
  glm::vec3 p[2] = {this->minPoint, this->maxPoint};

  AABB aabb = AABB::invalid();
  for(int i=0; i<8; ++i)
  {
    glm::ivec3 p_index((i&4) > 0,
                       (i&2) > 0,
                       (i&1) > 0);

    glm::vec3 v = coordFrame.transform_point(glm::vec3(p[p_index.x].x,
                                                       p[p_index.y].y,
                                                       p[p_index.z].z));

    aabb |= v;
  }

  return aabb;
}

AABB AABB::ensureValid() const
{
  AABB aabb = *this;

  if(!isValid())
  {
    aabb.minPoint = glm::vec3(-1);
    aabb.maxPoint = glm::vec3(1);
  }

  return aabb;
}

QDebug operator<<(QDebug d, const AABB& aabb)
{
  return d << "AABB{min:"<<aabb.minPoint<<", max:"<<aabb.maxPoint<<"}";
}


} // namespace scene
} // namespace glrt
