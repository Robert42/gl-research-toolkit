#include <glrt/scene/resources/resource-loader.h>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {
namespace resources {


using AngelScriptIntegration::AngelScriptCheck;


void pass_arg_to_angelscript(AngelScript::asIScriptContext*context, int i, StaticMeshLoader* value)
{
  context->SetArgObject(i, value);
}

void pass_arg_to_angelscript(AngelScript::asIScriptContext*context, int i, ResourceLoader* value)
{
  context->SetArgObject(i, value);
}

// ======== StaticMeshLoader ===================================================

StaticMeshLoader::StaticMeshLoader()
{
}

StaticMeshLoader::~StaticMeshLoader()
{
}

void StaticMeshLoader::loadStaticMesh(const StaticMeshUuid& uuid, const std::string& filepath)
{
  AngelScriptIntegration::ConfigCallScript config;
  config.accessMask = ACCESS_MASK_RESOURCE_LOADING;

  AngelScriptIntegration::callScriptExt<void>(angelScriptEngine, filepath.c_str(), "void main(StaticMeshLoader@, StaticMeshUuid &in)", "static-mesh-loader", config, this, &uuid);
}

void StaticMeshLoader::loadStaticMesh(const StaticMeshUuid& uuid, const AngelScript::CScriptArray* indices, const AngelScript::CScriptArray* vertices)
{
  if(indices->GetArrayObjectType() != angelScriptEngine->GetObjectTypeByName("uint16"))
    throw GLRT_EXCEPTION("ResourceLoader::loadStaticMesh accepts an vertex array of the type array<uint16>");
  if(vertices->GetArrayObjectType() != angelScriptEngine->GetObjectTypeByName("float"))
    throw GLRT_EXCEPTION("ResourceLoader::loadStaticMesh accepts an vertex array of the type array<float>");

  loadStaticMesh(uuid,
                 reinterpret_cast<const StaticMeshData::index_type*>(indices->At(0)), indices->GetSize(),
                 reinterpret_cast<const StaticMeshData::Vertex*>(vertices->At(0)), vertices->GetSize()*sizeof(StaticMeshData::Vertex)/sizeof(float));
}

void StaticMeshLoader::loadStaticMesh(const StaticMeshUuid& uuid, const StaticMeshData& data)
{
  loadStaticMesh(uuid, data.indices.data(), data.indices.length(), data.vertices.data(), data.vertices.length());
}

void StaticMeshLoader::loadStaticMesh(const StaticMeshUuid& uuid, const StaticMeshData::index_type* indices, size_t numIndices, const StaticMeshData::Vertex* vertices, size_t numVertices)
{
  loadStaticMeshImpl(uuid, indices, numIndices, vertices, numVertices);
}

void StaticMeshLoader::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("StaticMeshLoader", sizeof(StaticMeshLoader), AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("StaticMeshLoader", "void loadStaticMesh(Uuid &in uuid, array<uint16> &in indices, array<float> &in vertices)", AngelScript::asMETHODPR(StaticMeshLoader, loadStaticMesh, (const StaticMeshUuid& uuid, const AngelScript::CScriptArray* indices, const AngelScript::CScriptArray* vertices), void), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


// ======== ResourceLoader =====================================================


ResourceLoader::ResourceLoader(ResourceIndex* index)
  : index(*index)
{
}

ResourceLoader::~ResourceLoader()
{
}

void ResourceLoader::startLoadingFromFile(const QUuid& uuid)
{
  Q_ASSERT(index.classInvariant());

  // Hmm, this is now threadsave
  if(index.isLoaded(uuid) || index.isLoading(uuid))
    return; // already loaded/loading, nothing left to do

  if(!index.unloadedRessources.contains(uuid))
    throw GLRT_EXCEPTION(QString("Can't load an unregistered ressource"));

  loadResourceFromFile(uuid, false);
  index.unloadedRessources.remove(uuid);
  index.loadingRessources.insert(uuid);
  // # TODO: how to get notified, if the ressource is actally laoded?

  Q_ASSERT(index.classInvariant());
}

void ResourceLoader::loadNowFromFile(const QUuid& uuid)
{
  Q_ASSERT(index.classInvariant());

  // Hmm, this is now threadsave
  if(index.isLoaded(uuid))
    return; // already loaded, nothing left to do

  index.unloadedRessources.remove(uuid);
  index.loadingRessources.insert(uuid);
  // # TODO: how to get notified, if the ressource is actally laoded?

  if(index.isLoading(uuid) || index.unloadedRessources.contains(uuid))
    loadResourceFromFile(uuid, true);
  else
    Q_UNREACHABLE();

  Q_ASSERT(index.classInvariant());
}


void ResourceLoader::loadResourceFromFile(const QUuid& uuid, bool loadNow)
{
  index._loadResource(this, uuid, loadNow);
}


} // namespace resources
} // namespace scene
} // namespace glrt

