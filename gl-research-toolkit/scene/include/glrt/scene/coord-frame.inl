#ifndef GLRT_SCENE_COORDFRAME_INL
#define GLRT_SCENE_COORDFRAME_INL

#include "coord-frame.h"

namespace glrt {
namespace scene {


inline void CoordFrame::_concatenate(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                                     const glm::vec3& aPosition, const glm::quat& aOrientation, float aScaleFactor,
                                     const glm::vec3& bPosition, const glm::quat& bOrientation, float bScaleFactor)
{
  // Do the positition first, because it's possible, that &aScaleFactor==outScaleFactor and &aScaleFactor==outOrientation.
  // By calculating the position first the original values are used.
  *outPosition = aPosition + aScaleFactor * (aOrientation * bPosition);
  *outScaleFactor = aScaleFactor * bScaleFactor;
  *outOrientation = aOrientation * bOrientation;
}

inline void CoordFrame::_coordinateFromMatrix(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                                              const glm::mat4& transform)
{
  *outScaleFactor = (glm::length(transform[0].xyz()) + glm::length(transform[1].xyz()) + glm::length(transform[2].xyz())) / 3.f;
  *outOrientation = glm::quat(transform);
  *outPosition = transform[3].xyz();
}

inline void CoordFrame::_transform_point(glm::vec3* outPoint,
                                         const glm::vec3& position, const glm::quat& orientation, float scaleFactor,
                                         const glm::vec3& inPoint)
{
  *outPoint = position + scaleFactor * (orientation * inPoint);
}

inline void CoordFrame::_transform_direction(glm::vec3* outDirection,
                                             const glm::quat& orientation,
                                             const glm::vec3& inDirection)
{
  *outDirection = orientation * inDirection;
}


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_COORDFRAME_INL
