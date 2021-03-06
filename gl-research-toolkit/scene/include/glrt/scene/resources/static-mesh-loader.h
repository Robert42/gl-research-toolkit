#ifndef GLRT_SCENE_RESOURCE_STATICMESHLOADER_H
#define GLRT_SCENE_RESOURCE_STATICMESHLOADER_H

#include <glrt/toolkit/uuid.h>

#include "resource-index.h"

#include "static-mesh.h"

#include <sdk/add_on/scriptarray/scriptarray.h>

namespace glrt {
namespace scene {
namespace resources {


class StaticMeshLoader : public QObject
{
  Q_OBJECT
public:
  typedef glrt::scene::AABB ABB;

  StaticMeshLoader();
  virtual ~StaticMeshLoader();

  StaticMeshLoader(const StaticMeshLoader&) = delete;
  StaticMeshLoader(StaticMeshLoader&&) = delete;
  StaticMeshLoader& operator=(const StaticMeshLoader&) = delete;
  StaticMeshLoader& operator=(StaticMeshLoader&&) = delete;

  virtual void removeUnusedStaticMeshes(QSet<Uuid<StaticMesh>> usedStaticMeshes) = 0;

  void loadStaticMesh(const Uuid<StaticMesh>& uuid, const ResourceManager* resourceManager);
  void loadStaticMesh(const Uuid<StaticMesh>& uuid, const std::string& filepath);
  void loadStaticMesh(const Uuid<StaticMesh>& uuid, const AngelScript::CScriptArray* indices, const AngelScript::CScriptArray* vertices);
  void loadStaticMesh(const Uuid<StaticMesh>& uuid, const StaticMesh& data);
  void loadStaticMesh(const Uuid<StaticMesh>& uuid, const StaticMesh::index_type* indices, size_t numIndices, const StaticMesh::Vertex* vertices, size_t numVertices);

  static void registerAngelScriptAPI();

  AABB staticMeshAABB(const Uuid<StaticMesh>& uuid, const ResourceManager* resourceManager, const AABB& fallback = AABB::invalid());

protected:
  virtual void loadStaticMeshImpl(const Uuid<StaticMesh>& uuid, const StaticMesh::index_type* indices, size_t numIndices, const StaticMesh::Vertex* vertices, size_t numVertices) = 0;
  virtual bool isAlreadyLoaded(const Uuid<StaticMesh>& uuid) const = 0;
  virtual AABB aabbForAlreadyLoaded(const Uuid<StaticMesh>& uuid) const = 0;
};


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCE_STATICMESHLOADER_H
