#include <glrt/scene/resources/resource-collector.h>
#include <glrt/scene/resources/resource-index.h>

namespace glrt {
namespace scene {
namespace resources {

QList<Uuid<Scene> > allRegisteredScenes(const ResourceManager& resourceManager)
{
  QList<Uuid<Scene> > uuids;
  for(const ResourceIndex* index : resourceManager.allIndices())
    uuids.append(index->sceneFiles.keys());
  return uuids;
}

} // namespace resources
} // namespace scene
} // namespace glrt
