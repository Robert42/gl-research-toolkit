#include <glrt/scene/resources/resource-loader.h>

namespace glrt {
namespace scene {
namespace resource {


ResourceLoader::ResourceLoader()
{
}

void ResourceLoader::startLoading(const QUuid& uuid)
{/* #TODO
  Q_ASSERT(this->classInvariant());

  if(isLoaded(uuid) || isLoading(uuid))
    return; // already loaded/loading, nothing left to do

  if(!unloadedRessources.contains(uuid))
    throw GLRT_EXCEPTION(QString("Can't load an unregistered ressource"));

  Q_ASSERT(this->classInvariant());
  */
}

void ResourceLoader::loadNow(const QUuid& uuid)
{/* #TODO
  Q_ASSERT(this->classInvariant());

  if(isLoaded(uuid))
    return; // already loaded, nothing left to do

  if(unloadedRessources.contains(uuid))
  else if(isLoading(uuid))
    throw GLRT_EXCEPTION(QString("Can't load an unregistered ressource"));

  Q_ASSERT(this->classInvariant());
  */
}


} // namespace resource
} // namespace scene
} // namespace glrt

