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

QString ResourceManager::labelForResourceUuid(const QUuid& uuid, const QString& fallback)
{
  return indexForResourceUuid(uuid)->labelForUuid(uuid, fallback);
}

QString ResourceManager::labelForResourceUuid(const QUuid& uuid)
{
  return indexForResourceUuid(uuid)->labelForUuid(uuid);
}


} // namespace resources
} // namespace scene
} // namespace glrt

