#ifndef GLRT_SCENE_COORDFRAME_H
#define GLRT_SCENE_COORDFRAME_H

#include <glrt/dependencies.h>

namespace glrt {
namespace scene {

struct CoordFrame final
{
  glm::vec3 position;
  float scaleFactor;
  glm::quat orientation;

  CoordFrame(const glm::ctor);
  explicit CoordFrame(const glm::vec3& position = glm::vec3(0),
                      const glm::quat& orientation = glm::quat::IDENTITY,
                      float scaleFactor = 1.f);
  CoordFrame(const glm::mat4& transformation);

  CoordFrame& operator *=(const CoordFrame& other);

  CoordFrame operator *(const CoordFrame& other) const;
  glm::vec3 operator *(const glm::vec3& point) const;

  glm::vec3 transform_point(const glm::vec3& point) const;
  glm::vec3 transform_direction(const glm::vec3& point) const;

  static void _concatenate(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                           const glm::vec3& aPosition, const glm::quat& aOrientation, float aScaleFactor,
                           const glm::vec3& bPosition, const glm::quat& bOrientation, float bScaleFactor);
  static void _coordinateFromMatrix(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                                    const glm::mat4& transform);
  static void _transform_point(glm::vec3* outPoint,
                               const glm::vec3& position, const glm::quat& orientation, float scaleFactor,
                               const glm::vec3& inPoint);
  static void _transform_direction(glm::vec3* outDirection,
                                   const glm::quat& orientation, float scaleFactor,
                                   const glm::vec3& inDirection);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_COORDFRAME_H
