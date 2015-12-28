#include <glrt/scene/resources/static-mesh-data.h>

namespace glrt {
namespace scene {
namespace resources {

bool StaticMeshData::isIndexed() const
{
  return not indices.isEmpty();
}

} // namespace resources
} // namespace scene
} // namespace glrt

