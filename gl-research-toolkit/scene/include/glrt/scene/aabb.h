#ifndef GLRT_SCENE_AABB_H
#define GLRT_SCENE_AABB_H

#include "coord-frame.h"

namespace glrt {
namespace scene {

struct AABB
{
  glm::vec3 minPoint;
  glm::vec3 maxPoint;

  bool isInf() const {return glm::any(glm::isinf(minPoint)) || glm::any(glm::isinf(maxPoint));}
  bool isNan() const {return glm::any(glm::isnan(minPoint)) || glm::any(glm::isnan(maxPoint));}
  bool isValid() const{return !isInf() && !isNan();}

  void operator |= (const AABB& other);
  void operator |= (const glm::vec3& other);

  static AABB invalid();

  static AABB fromVertices(const glm::vec3* vertices, int numVertices);
  static AABB fromVertices(const glm::vec3* vertices, int numVertices, size_t stride);
  AABB aabbOfTransformedBoundingBox(const CoordFrame& coordFrame) const;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_AABB_H
