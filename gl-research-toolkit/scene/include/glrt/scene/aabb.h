#ifndef GLRT_SCENE_AABB_H
#define GLRT_SCENE_AABB_H

#include "coord-frame.h"

namespace glrt {
namespace scene {

struct AABB
{
  glm::vec3 minPoint;
  glm::vec3 maxPoint;

  AABB aabbOfTransformedBoundingBox(const CoordFrame& coordFrame) const;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_AABB_H
