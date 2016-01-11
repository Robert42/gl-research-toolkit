#include <glrt/scene/resources/static-mesh-loader.h>

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

void StaticMeshLoader::loadStaticMesh(Uuid<StaticMeshData> uuid, const std::string& filepath)
{
  AngelScriptIntegration::ConfigCallScript config;
  config.accessMask = ACCESS_MASK_RESOURCE_LOADING;

  AngelScriptIntegration::callScriptExt<void>(angelScriptEngine, filepath.c_str(), "void main(StaticMeshLoader@, Uuid<StaticMesh> &in)", "static-mesh-loader", config, this, &uuid);
}

void StaticMeshLoader::loadStaticMesh(const Uuid<StaticMeshData>& uuid, const AngelScript::CScriptArray* _indices, const AngelScript::CScriptArray* _vertices)
{
  size_t numIndices = _indices->GetSize();
  size_t numVertices = _vertices->GetSize();

  // _vertices is an array iof the type float, but we need the number of vertices
  numVertices = (numVertices*sizeof(float)) / sizeof(StaticMeshData::Vertex);

  const StaticMeshData::index_type* indices = reinterpret_cast<const StaticMeshData::index_type*>(_indices->At(0));
  const StaticMeshData::Vertex* vertices = reinterpret_cast<const StaticMeshData::Vertex*>(_vertices->At(0));

  loadStaticMesh(uuid, indices, numIndices, vertices, numVertices);

  _indices->Release();
  _vertices->Release();
}

void StaticMeshLoader::loadStaticMesh(const Uuid<StaticMeshData>& uuid, const StaticMeshData& data)
{
  loadStaticMesh(uuid, data.indices.data(), data.indices.length(), data.vertices.data(), data.vertices.length());
}

void StaticMeshLoader::loadStaticMesh(const Uuid<StaticMeshData>& uuid, const StaticMeshData::index_type* indices, size_t numIndices, const StaticMeshData::Vertex* vertices, size_t numVertices)
{
  loadStaticMeshImpl(uuid, indices, numIndices, vertices, numVertices);
}

void StaticMeshLoader::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("StaticMeshLoader", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("StaticMeshLoader", "void loadStaticMesh(const Uuid<StaticMesh> &in uuid, const array<uint16>@ indices, const array<float>@ vertices)", AngelScript::asMETHODPR(StaticMeshLoader, loadStaticMesh, (const Uuid<StaticMeshData>& uuid, const AngelScript::CScriptArray* indices, const AngelScript::CScriptArray* vertices), void), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace resources
} // namespace scene
} // namespace glrt

