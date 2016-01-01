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


} // namespace scene
} // namespace glrt

