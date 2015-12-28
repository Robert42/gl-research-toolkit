#include <glrt/scene/resources/resource-loader.h>

namespace glrt {
namespace scene {
namespace resources {


ResourceLoader::ResourceLoader(ResourceIndex* index)
  : index(*index)
{
}

void ResourceLoader::startLoadingFromFile(const QUuid& uuid)
{
  Q_ASSERT(index.classInvariant());

  // Hmm, this is now threadsave
  if(index.isLoaded(uuid) || index.isLoading(uuid))
    return; // already loaded/loading, nothing left to do

  if(!index.unloadedRessources.contains(uuid))
    throw GLRT_EXCEPTION(QString("Can't load an unregistered ressource"));

  loadResourceFromFile(uuid, false);
  index.unloadedRessources.remove(uuid);
  index.loadingRessources.insert(uuid);
  // # TODO: how to get notified, if the ressource is actally laoded?

  Q_ASSERT(index.classInvariant());
}

void ResourceLoader::loadNowFromFile(const QUuid& uuid)
{
  Q_ASSERT(index.classInvariant());

  // Hmm, this is now threadsave
  if(index.isLoaded(uuid))
    return; // already loaded, nothing left to do

  index.unloadedRessources.remove(uuid);
  index.loadingRessources.insert(uuid);
  // # TODO: how to get notified, if the ressource is actally laoded?

  if(index.isLoading(uuid) || index.unloadedRessources.contains(uuid))
    loadResourceFromFile(uuid, true);
  else
    Q_UNREACHABLE();

  Q_ASSERT(index.classInvariant());
}


void ResourceLoader::loadResourceFromFile(const QUuid& uuid, bool loadNow)
{
  index._loadResource(this, uuid, loadNow);
}


} // namespace resources
} // namespace scene
} // namespace glrt

