#include <glrt/scene/coord-frame.h>

namespace glrt {
namespace scene {

CoordFrame::CoordFrame(glm::ctor ctor)
  : position(ctor),
    orientation(ctor)
{
}

CoordFrame::CoordFrame(const glm::vec3& position, const glm::quat& orientation, float scaleFactor)
  : position(position),
    scaleFactor(scaleFactor),
    orientation(orientation)
{
}

CoordFrame::CoordFrame(const glm::mat4& transformation)
{
  _coordinateFromMatrix(&this->position, &this->orientation, &this->scaleFactor, transformation);
}

CoordFrame& CoordFrame::operator *=(const CoordFrame& other)
{
  _concatenate(&this->position, &this->orientation, &this->scaleFactor, this->position, this->orientation, this->scaleFactor, other.position, other.orientation, other.scaleFactor);
  return *this;
}

CoordFrame CoordFrame::operator *(const CoordFrame& other) const
{
  CoordFrame f = *this;

  return f *= other;
}


glm::vec3 CoordFrame::transform_point(const glm::vec3& point) const
{
  glm::vec3 transformed_point;
  _transform_point(&transformed_point,
                   this->position, this->orientation, this->scaleFactor,
                   point);
  return transformed_point;
}

glm::vec3 CoordFrame::transform_direction(const glm::vec3& point) const
{
  glm::vec3 transformed_direction;
  _transform_direction(&transformed_direction,
                       this->position, this->orientation, this->scaleFactor,
                       point);
  return transformed_direction;
}


void CoordFrame::_concatenate(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                         const glm::vec3& aPosition, const glm::quat& aOrientation, float aScaleFactor,
                         const glm::vec3& bPosition, const glm::quat& bOrientation, float bScaleFactor)
{
  // #IMPLEMENT
}

void CoordFrame::_coordinateFromMatrix(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                                  const glm::mat4& transform)
{
  // #IMPLEMENT
}

void CoordFrame::_transform_point(glm::vec3* outPoint,
                             const glm::vec3& position, const glm::quat& orientation, float scaleFactor,
                             const glm::vec3& inPoint)
{
  // #IMPLEMENT
}

void CoordFrame::_transform_direction(glm::vec3* outDirection,
                                 const glm::vec3& position, const glm::quat& orientation, float scaleFactor,
                                 const glm::vec3& inDirection)
{
  // #IMPLEMENT
}

} // namespace scene
} // namespace glrt

