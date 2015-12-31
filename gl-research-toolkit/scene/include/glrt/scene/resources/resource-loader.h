#ifndef GLRT_SCENE_RESOURCE_RESOURCELOADER_H
#define GLRT_SCENE_RESOURCE_RESOURCELOADER_H

#include <glrt/toolkit/uuid.h>

#include "resource-index.h"

#include "static-mesh-data.h"
#include "material-data.h"

#include <sdk/add_on/scriptarray/scriptarray.h>

namespace glrt {
namespace scene {
namespace resources {


class StaticMeshLoader
{
public:
  StaticMeshLoader();
  virtual ~StaticMeshLoader();

  StaticMeshLoader(const StaticMeshLoader&) = delete;
  StaticMeshLoader(StaticMeshLoader&&) = delete;
  StaticMeshLoader& operator=(const StaticMeshLoader&) = delete;
  StaticMeshLoader& operator=(StaticMeshLoader&&) = delete;

  void loadStaticMesh(Uuid<StaticMeshData> uuid, const std::string& filepath);
  void loadStaticMesh(const Uuid<StaticMeshData>& uuid, const AngelScript::CScriptArray* indices, const AngelScript::CScriptArray* vertices);
  void loadStaticMesh(const Uuid<StaticMeshData>& uuid, const StaticMeshData& data);
  void loadStaticMesh(const Uuid<StaticMeshData>& uuid, const StaticMeshData::index_type* indices, size_t numIndices, const StaticMeshData::Vertex* vertices, size_t numVertices);

  static void registerAngelScriptAPI();

protected:
  virtual void loadStaticMeshImpl(const Uuid<StaticMeshData>& uuid, const StaticMeshData::index_type* indices, size_t numIndices, const StaticMeshData::Vertex* vertices, size_t numVertices) = 0;
};


class ResourceLoader : public StaticMeshLoader
{
public:
  // The given index instance must exist longer than the new ResourceLoader instance
  ResourceLoader(ResourceIndex* index);
  virtual ~ResourceLoader();

  ResourceLoader(const ResourceLoader&) = delete;
  ResourceLoader(ResourceLoader&&) = delete;
  ResourceLoader& operator=(const ResourceLoader&) = delete;
  ResourceLoader& operator=(ResourceLoader&&) = delete;

  void startLoadingFromFile(const QUuid& uuid);
  void loadNowFromFile(const QUuid& uuid);

  void loadResourceFromFile(const QUuid& uuid, bool loadNow);

private:
  ResourceIndex& index;
};


} // namespace resources
} // namespace scene
} // namespace glrt

#include "resource-loader.inl"

#endif // GLRT_SCENE_RESOURCE_RESOURCELOADER_H
