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

} // namespace scene
} // namespace glrt

