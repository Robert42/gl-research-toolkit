#include <glrt/renderer/static-mesh-buffer-manager.h>

namespace glrt {
namespace renderer {

StaticMeshBufferManager::StaticMeshBufferManager(glrt::scene::resources::ResourceManager* resourceManager)
  : resourceManager(resourceManager)
{

}

StaticMeshBufferManager::~StaticMeshBufferManager()
{
  for(StaticMeshBuffer* buffer : staticMeshes)
    delete buffer;
}

void StaticMeshBufferManager::removeUnusedStaticMeshes(QSet<Uuid<StaticMesh>> usedStaticMeshes)
{
  QList<Uuid<StaticMesh>> allMeshes = staticMeshes.keys();
  for(Uuid<StaticMesh> staticMesh : allMeshes)
  {
    if(usedStaticMeshes.contains(staticMesh))
      continue;

    delete staticMeshes[staticMesh];
    staticMeshes.remove(staticMesh);
  }
}

bool StaticMeshBufferManager::isAlreadyLoaded(const Uuid<StaticMesh>& uuid) const
{
  return staticMeshes.contains(uuid);
}

StaticMeshBuffer* StaticMeshBufferManager::meshForUuid(const Uuid<StaticMesh>& uuid)
{
  StaticMeshBuffer* buffer = staticMeshes.value(uuid, nullptr);

  if(buffer == nullptr)
  {
    resourceManager->loadStaticMesh(uuid);
    buffer = staticMeshes.value(uuid, nullptr);
  }

  if(buffer == nullptr)
    throw GLRT_EXCEPTION(QString("Can't find static-mesh for the uuid %0").arg(uuid.toString()));

  return buffer;
}


void StaticMeshBufferManager::loadStaticMeshImpl(const Uuid<StaticMesh>& uuid, const StaticMesh::index_type* indices, size_t numIndices, const StaticMesh::Vertex* vertices, size_t numVertices)
{
  if(isAlreadyLoaded(uuid))
    return;

  StaticMeshBuffer newBuffer = StaticMeshBuffer::createIndexed(indices, numIndices, vertices, numVertices, numIndices!=0);

  staticMeshes.insert(uuid, new StaticMeshBuffer(std::move(newBuffer)));
}


} // namespace renderer
} // namespace glrt
