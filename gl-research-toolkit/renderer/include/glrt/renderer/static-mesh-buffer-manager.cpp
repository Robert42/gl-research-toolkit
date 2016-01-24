#include "static-mesh-buffer-manager.h"

namespace glrt {
namespace renderer {

StaticMeshBufferManager::StaticMeshBufferManager()
{

}


StaticMeshBuffer* StaticMeshBufferManager::meshForUuid(const Uuid<StaticMesh>& uuid)
{
  StaticMeshBuffer* buffer = staticMeshes.value(uuid, nullptr);

  if(buffer == nullptr)
    throw GLRT_EXCEPTION(QString("Can't find static-mesh for the uuid %0").arg(uuid.toString()));

  return buffer;
}


void StaticMeshBufferManager::loadStaticMeshImpl(const Uuid<StaticMesh>& uuid, const StaticMesh::index_type* indices, size_t numIndices, const StaticMesh::Vertex* vertices, size_t numVertices)
{
  StaticMeshBuffer newBuffer = StaticMeshBuffer::createIndexed(indices, numIndices, vertices, numVertices, numIndices!=0);

  StaticMeshBuffer* buffer = staticMeshes.value(uuid, nullptr);
  if(buffer)
    *buffer = std::move(newBuffer);
  else
    staticMeshes[uuid] = buffer = new StaticMeshBuffer(std::move(newBuffer));
}


} // namespace renderer
} // namespace glrt
