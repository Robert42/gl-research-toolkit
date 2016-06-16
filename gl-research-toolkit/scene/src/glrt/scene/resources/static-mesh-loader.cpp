#include <glrt/scene/resources/static-mesh-loader.h>
#include <glrt/scene/resources/static-mesh-file.h>
#include <glrt/scene/resources/resource-manager.h>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {
namespace resources {


using AngelScriptIntegration::AngelScriptCheck;


// ======== StaticMeshLoader ===================================================

StaticMeshLoader::StaticMeshLoader()
{
}

StaticMeshLoader::~StaticMeshLoader()
{
}

void StaticMeshLoader::loadStaticMesh(const Uuid<StaticMesh>& uuid, const ResourceManager* resourceManager)
{
  if(isAlreadyLoaded(uuid))
    return;

  QString file = resourceManager->staticMeshFileForUuid(uuid);
  if(!file.isEmpty())
    loadStaticMesh(uuid, file.toStdString());
}

void StaticMeshLoader::loadStaticMesh(const Uuid<StaticMesh>& uuid, const std::string& filepath)
{
  if(isAlreadyLoaded(uuid))
    return;

  StaticMeshFile file;
  file.load(QString::fromStdString(filepath));

  this->loadStaticMesh(uuid, file.staticMesh);
}

void StaticMeshLoader::loadStaticMesh(const Uuid<StaticMesh>& uuid, const StaticMesh& data)
{
  loadStaticMesh(uuid, data.indices.data(), data.indices.length(), data.vertices.data(), data.vertices.length());
}

void StaticMeshLoader::loadStaticMesh(const Uuid<StaticMesh>& uuid, const StaticMesh::index_type* indices, size_t numIndices, const StaticMesh::Vertex* vertices, size_t numVertices)
{
  loadStaticMeshImpl(uuid, indices, numIndices, vertices, numVertices);
}

AABB StaticMeshLoader::staticMeshAABB(const Uuid<StaticMesh>& uuid, const ResourceManager* resourceManager, const AABB& fallback)
{
  if(isAlreadyLoaded(uuid))
    return aabbForAlreadyLoaded(uuid);

  QString filepath = resourceManager->staticMeshFileForUuid(uuid);
  if(filepath.isEmpty())
    return fallback;

  StaticMeshFile file;
  file.load(filepath);

  return file.staticMesh.boundingBox();
}

void StaticMeshLoader::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("StaticMeshLoader", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("StaticMeshLoader",
                                              "void loadStaticMesh(const Uuid<StaticMesh> &in uuid, const string &in file)",
                                              AngelScript::asMETHODPR(StaticMeshLoader, loadStaticMesh, (const Uuid<StaticMesh>& uuid, const std::string& filepath), void),
                                              AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace resources
} // namespace scene
} // namespace glrt

