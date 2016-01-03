#include <glrt/scene/object.h>

namespace glrt {
namespace scene {


Object::Object(const QUuid& uuid)
  : uuid(uuid)
{
}

QString Object::get_label() const
{
  return uuid.toString(); // #TODO: use the scene to get the resourceIndex to get the label for this uuid? Or simply store the label with the object?
}


} // namespace scene
} // namespace glrt

