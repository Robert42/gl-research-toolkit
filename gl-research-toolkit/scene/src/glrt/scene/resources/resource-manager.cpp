#include <glrt/scene/resources/resource-manager.h>

namespace glrt {
namespace scene {
namespace resources {


ResourceManager::ResourceManager(StaticMeshLoader* staticMeshLoader)
  : staticMeshLoader(*staticMeshLoader)
{
}

ResourceManager::~ResourceManager()
{
}


} // namespace resources
} // namespace scene
} // namespace glrt

