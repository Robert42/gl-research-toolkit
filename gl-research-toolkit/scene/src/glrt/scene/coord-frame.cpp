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

CoordFrame::CoordFrame(const aiMatrix4x4& transformation)
  : CoordFrame(to_glm_mat4(transformation))
{
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
  glm::mat4 m;

  _to_mat4(reinterpret_cast<float*>(&m),
           this->position, this->orientation, this->scaleFactor);
  return m;
}

CoordFrame CoordFrame::inverse() const
{
  CoordFrame i;
  _inverse(&i.position, &i.orientation, &i.scaleFactor,
           this->position, this->orientation, this->scaleFactor);
  return i;
}

QString CoordFrame::as_angelscript_fast() const
{
  return QString("CoordFrame(vec3(%0, %1, %2), quat(%3, %4, %5, %6), %7)").arg(this->position.x).arg(this->position.y).arg(this->position.z).arg(this->orientation.x).arg(this->orientation.y).arg(this->orientation.z).arg(this->orientation.w).arg(this->scaleFactor);
}

QDebug operator<<(QDebug debug, const CoordFrame& coordFrame)
{
  return debug << "CoordFrame(position: " << coordFrame.position << ", orientation: " << coordFrame.orientation << ", scale: " << coordFrame.scaleFactor << ")";
}

} // namespace scene
} // namespace glrt

