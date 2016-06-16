#ifndef GLRT_RENDERER_STATICMESHBUFFERMANAGER_H
#define GLRT_RENDERER_STATICMESHBUFFERMANAGER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/static-mesh-buffer.h>
#include <glrt/scene/resources/static-mesh-loader.h>
#include <glrt/scene/resources/resource-manager.h>

namespace glrt {
namespace renderer {

class StaticMeshBufferManager final : public StaticMeshLoader
{
public:
  StaticMeshBufferManager(glrt::scene::resources::ResourceManager* resourceManager);
  ~StaticMeshBufferManager();

  StaticMeshBuffer* meshForUuid(const Uuid<StaticMesh>& uuid);

  void removeUnusedStaticMeshes(QSet<Uuid<StaticMesh>> usedStaticMeshes) override;
  bool isAlreadyLoaded(const Uuid<StaticMesh>& uuid) const override;

private:
  QHash<Uuid<StaticMesh>, StaticMeshBuffer*> staticMeshes;
  glrt::scene::resources::ResourceManager* resourceManager;

  void loadStaticMeshImpl(const Uuid<StaticMesh>& uuid, const StaticMesh::index_type* indices, size_t numIndices, const StaticMesh::Vertex* vertices, size_t numVertices) override;

  scene::AABB aabbForAlreadyLoaded(const Uuid<StaticMesh>& uuid) const override;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESHBUFFERMANAGER_H
