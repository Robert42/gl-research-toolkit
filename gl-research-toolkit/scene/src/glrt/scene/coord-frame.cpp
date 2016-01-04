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

glm::vec3 CoordFrame::operator *(const glm::vec3& point) const
{
  return this->transform_point(point);
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
                       this->orientation,
                       point);
  return transformed_direction;
}

glm::mat4 CoordFrame::toMat4() const
{
  // #TODO make this also inline and optimize this
  return glm::translate(glm::mat4(1), this->position) * glm::toMat4(this->orientation) * glm::scale(glm::mat4(1), glm::vec3(this->scaleFactor));
}

CoordFrame CoordFrame::inverse() const
{
  CoordFrame i;
  _inverse(&i.position, &i.orientation, &i.scaleFactor,
           this->position, this->orientation, this->scaleFactor);
  return i;
}

QDebug operator<<(QDebug debug, const CoordFrame& coordFrame)
{
  return debug << "CoordFrame(position: " << coordFrame.position << ", orientation: " << coordFrame.orientation << ", scale: " << coordFrame.scaleFactor << ")";
}

} // namespace scene
} // namespace glrt

