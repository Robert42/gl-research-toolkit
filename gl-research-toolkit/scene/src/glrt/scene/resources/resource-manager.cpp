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
  staticMeshLoader->setParent(this);
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

void ResourceManager::loadStaticMesh(const Uuid<StaticMesh>& uuid)
{
  this->staticMeshLoader.loadStaticMesh(uuid, this);
}

QString ResourceManager::labelForResourceUuid(const QUuid& uuid) const
{
  return labelForResourceUuid(uuid, uuid.toString());
}

LightSource ResourceManager::lightSourceForUuid(const Uuid<LightSource>& uuid) const
{
  return indexForResourceUuid(uuid)->lightSources.value(uuid, Index::fallback.lightSources[uuids::fallbackLight]);
}

Material ResourceManager::materialForUuid(const Uuid<Material>& uuid) const
{
  return indexForResourceUuid(uuid)->materials.value(uuid, Index::fallback.materials[uuids::fallbackMaterial]);
}

QString ResourceManager::staticMeshFileForUuid(const Uuid<StaticMesh>& uuid, const QString& fallback) const
{
  return indexForResourceUuid(uuid)->staticMeshAssetsFiles.value(uuid, fallback);
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
  if(foreachIndexImpl(lambda))
    return;

  for(const SceneLayer* sceneLayer : _sceneLayers)
    if(lambda(&sceneLayer->index))
      return;

  lambda(&Index::fallback);
}

QList<const ResourceIndex*> ResourceManager::allIndices() const
{
  QList<const Index*> all;

  foreachIndex([&all](const Index* index){all.append(index);return false;});

  return all;
}

QString ResourceManager::labelForResourceUuid(const QUuid& uuid, const QString& fallback) const
{
  QString returnedLabel = fallback;

  foreachIndex([&returnedLabel, &uuid](const Index* index){
    auto i = index->labels.find(uuid);
    if(i != index->labels.end())
    {
      returnedLabel = i.value();
      return true;
    }
    return false;
  });

  return returnedLabel;
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

