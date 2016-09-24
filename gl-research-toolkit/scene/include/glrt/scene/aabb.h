#ifndef GLRT_SCENE_AABB_H
#define GLRT_SCENE_AABB_H

#include "coord-frame.h"

namespace glrt {
namespace scene {

struct AABB
{
  glm::vec3 minPoint;
  padding<float, 1> _padding1;
  glm::vec3 maxPoint;
  padding<float, 1> _padding2;

  bool isInf() const {return glm::any(glm::isinf(minPoint)) || glm::any(glm::isinf(maxPoint));}
  bool isNan() const {return glm::any(glm::isnan(minPoint)) || glm::any(glm::isnan(maxPoint));}
  bool isValid() const{return !isInf() && !isNan() && all(greaterThan(maxPoint, minPoint));}

  glm::vec3 toUnitSpace(const glm::vec3& v) const;

  void operator |= (const AABB& other);
  void operator |= (const glm::vec3& other);

  static AABB invalid();

  static AABB fromVertices(const glm::vec3* vertices, int numVertices);
  static AABB fromVertices(const glm::vec3* vertices, int numVertices, size_t stride);
  AABB aabbOfTransformedBoundingBox(const CoordFrame& coordFrame) const;

  AABB ensureValid() const;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_AABB_H
