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

QList<Uuid<Scene> > ResourceManager::allRegisteredScenes()
{
  QList<Uuid<Scene> > uuids;
  for(const ResourceIndex* index : this->allIndices())
    uuids.append(index->sceneFiles.keys());
  return uuids;
}

QString ResourceManager::labelForResourceUuid(const QUuid& uuid, const QString& fallback) const
{
  return indexForResourceUuid(uuid)->labels.value(uuid, fallback);
}

QString ResourceManager::labelForResourceUuid(const QUuid& uuid) const
{
  return indexForResourceUuid(uuid)->labels.value(uuid, uuid.toString());
}

QString ResourceManager::sceneFileForUuid(const Uuid<Scene>& uuid, const QString& fallback) const
{
  return indexForResourceUuid(uuid)->sceneFiles.value(uuid, fallback);
}

QString ResourceManager::sceneLayerFileForUuid(const Uuid<SceneLayer>& uuid, const QString& fallback) const
{
  return indexForResourceUuid(uuid)->sceneLayerFiles.value(uuid, fallback);
}

void ResourceManager::foreachIndex(const std::function<bool(const Index* index)>& lambda) const
{
  foreachIndexImpl(lambda);
  lambda(&Index::fallback);
}

QList<const ResourceIndex*> ResourceManager::allIndices() const
{
  QList<const Index*> all;

  foreachIndex([&all](const Index* index){all.append(index);return false;});

  return all;
}

const ResourceIndex* ResourceManager::indexForResourceUuid(const QUuid& uuid, const Index* fallback) const
{
  const Index* returnedIndex = fallback;

  foreachIndex([&returnedIndex, &uuid](const Index* index){
    if(index->isRegistered(uuid))
    {
      returnedIndex = index;
      return true;
    }
    return false;
  });

  return returnedIndex;
}


} // namespace resources
} // namespace scene
} // namespace glrt

