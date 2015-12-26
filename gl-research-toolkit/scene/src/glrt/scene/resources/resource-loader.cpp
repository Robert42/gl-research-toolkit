#include <glrt/scene/resources/resource-loader.h>

namespace glrt {
namespace scene {
namespace resources {


ResourceLoader::ResourceLoader(ResourceIndex* index)
  : index(*index)
{
}

void ResourceLoader::startLoading(const QUuid& uuid)
{
  Q_ASSERT(index.classInvariant());

  // Hmm, this is now threadsave
  if(index.isLoaded(uuid) || index.isLoading(uuid))
    return; // already loaded/loading, nothing left to do

  if(!index.unloadedRessources.contains(uuid))
    throw GLRT_EXCEPTION(QString("Can't load an unregistered ressource"));

  _loadResource(uuid, false);
  index.unloadedRessources.remove(uuid);
  index.loadingRessources.insert(uuid);
  // # TODO: how to get notified, if the ressource is actally laoded?

  Q_ASSERT(index.classInvariant());
}

void ResourceLoader::loadNow(const QUuid& uuid)
{
  Q_ASSERT(index.classInvariant());

  // Hmm, this is now threadsave
  if(index.isLoaded(uuid))
    return; // already loaded, nothing left to do

  index.unloadedRessources.remove(uuid);
  index.loadingRessources.insert(uuid);
  // # TODO: how to get notified, if the ressource is actally laoded?

  if(index.isLoading(uuid) || index.unloadedRessources.contains(uuid))
    _loadResource(uuid, true);
  else
    Q_UNREACHABLE();

  Q_ASSERT(index.classInvariant());
}


void ResourceLoader::_loadResource(const QUuid& uuid, bool loadNow)
{
  // Hmm, this is not threadsave
  if(registeredStaticMeshFiles.contains(uuid))
  {
    StaticMeshUuid staticMesh(uuid, this);

    if(!index.isLoading(uuid)) // Already loading?
      loadStaticMesh(staticMesh, loadStaticMeshFromFile(registeredStaticMeshFiles[uuid]));
    if(loadNow)
      waitForStaticMeshToBeLoaded(staticMesh);
  }
}

void ResourceLoader::registerStaticMeshFile(const StaticMeshUuid& uuid, const QString& filename)
{
  registeredStaticMeshFiles[uuid.uuid()] = filename;
}


} // namespace resources
} // namespace scene
} // namespace glrt

