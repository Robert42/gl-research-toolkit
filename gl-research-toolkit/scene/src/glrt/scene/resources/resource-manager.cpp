#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/scene-layer.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;


resources::StaticMeshLoader* get_staticMeshLoader(resources::ResourceManager* resourceManager)
{
  return &resourceManager->staticMeshLoader;
}

// -------- ResourceManager ----------------------------------------------------


ResourceManager::ResourceManager(StaticMeshLoader* staticMeshLoader)
  : staticMeshLoader(*staticMeshLoader)
{
}

ResourceManager::~ResourceManager()
{
}

QList<Uuid<Scene> > ResourceManager::allRegisteredScenes()
{
  QList<Uuid<Scene> > uuids;
  for(const ResourceIndex* index : this->allIndices())
    uuids.append(index->sceneFiles.keys());
  return uuids;
}

QString ResourceManager::labelForResourceUuid(const QUuid& uuid, const QString& fallback) const
{
  return indexForResourceUuid(uuid)->labels.value(uuid, fallback);
}

QString ResourceManager::labelForResourceUuid(const QUuid& uuid) const
{
  return indexForResourceUuid(uuid)->labels.value(uuid, uuid.toString());
}

QString ResourceManager::sceneFileForUuid(const Uuid<Scene>& uuid, const QString& fallback) const
{
  return indexForResourceUuid(uuid)->sceneFiles.value(uuid, fallback);
}

QString ResourceManager::sceneLayerFileForUuid(const Uuid<SceneLayer>& uuid, const QString& fallback) const
{
  return indexForResourceUuid(uuid)->sceneLayerFiles.value(uuid, fallback);
}

void ResourceManager::foreachIndex(const std::function<bool(const Index* index)>& lambda) const
{
  foreachIndexImpl(lambda);

  for(const SceneLayer* sceneLayer : _sceneLayers)
    lambda(&sceneLayer->index);

  lambda(&Index::fallback);
}

QList<const ResourceIndex*> ResourceManager::allIndices() const
{
  QList<const Index*> all;

  foreachIndex([&all](const Index* index){all.append(index);return false;});

  return all;
}

const ResourceIndex* ResourceManager::indexForResourceUuid(const QUuid& uuid, const Index* fallback) const
{
  const Index* returnedIndex = fallback;

  foreachIndex([&returnedIndex, &uuid](const Index* index){
    if(index->isRegistered(uuid))
    {
      returnedIndex = index;
      return true;
    }
    return false;
  });

  return returnedIndex;
}

void ResourceManager::registerAngelScriptAPIDeclarations()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("ResourceManager", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void ResourceManager::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectMethod("ResourceManager", "StaticMeshLoader@ get_staticMeshLoader()", AngelScript::asFUNCTION(get_staticMeshLoader), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace resources
} // namespace scene
} // namespace glrt

