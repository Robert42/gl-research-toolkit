#ifndef GLRT_RENDERER_STATICMESHBUFFERMANAGER_H
#define GLRT_RENDERER_STATICMESHBUFFERMANAGER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/static-mesh-buffer.h>
#include <glrt/scene/resources/static-mesh-loader.h>

namespace glrt {
namespace renderer {

class StaticMeshBufferManager final : public StaticMeshLoader
{
public:
  StaticMeshBufferManager();

  StaticMeshBuffer* meshForUuid(const Uuid<StaticMesh>& uuid);

private:
  QHash<Uuid<StaticMesh>, StaticMeshBuffer*> staticMeshes;

  void loadStaticMeshImpl(const Uuid<StaticMesh>& uuid, const StaticMesh::index_type* indices, size_t numIndices, const StaticMesh::Vertex* vertices, size_t numVertices) override;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESHBUFFERMANAGER_H
