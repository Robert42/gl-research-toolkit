#ifndef GLRT_SCENE_RESOURCE_RESOURCELOADER_H
#define GLRT_SCENE_RESOURCE_RESOURCELOADER_H

#include "resource-uuid.h"
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

  void loadStaticMesh(StaticMeshUuid uuid, const std::string& filepath);
  void loadStaticMesh(const StaticMeshUuid& uuid, const AngelScript::CScriptArray* indices, const AngelScript::CScriptArray* vertices);
  void loadStaticMesh(const StaticMeshUuid& uuid, const StaticMeshData& data);
  void loadStaticMesh(const StaticMeshUuid& uuid, const StaticMeshData::index_type* indices, size_t numIndices, const StaticMeshData::Vertex* vertices, size_t numVertices);

  static void registerAngelScriptAPI();

protected:
  virtual void loadStaticMeshImpl(const StaticMeshUuid& uuid, const StaticMeshData::index_type* indices, size_t numIndices, const StaticMeshData::Vertex* vertices, size_t numVertices) = 0;
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

void pass_arg_to_angelscript(AngelScript::asIScriptContext*context, int i, StaticMeshLoader* value);
void pass_arg_to_angelscript(AngelScript::asIScriptContext*context, int i, ResourceLoader* value);

} // namespace resources
} // namespace scene
} // namespace glrt

#include "resource-uuid-loader.inl"

#endif // GLRT_SCENE_RESOURCE_RESOURCELOADER_H
