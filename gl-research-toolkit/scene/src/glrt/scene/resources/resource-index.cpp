#include <glrt/scene/resources/resource-index.h>
#include <glrt/scene/resources/resource-loader.h>
#include <glrt/scene/resources/asset-converter.h>
#include <QThread>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;

void convertSceneGraph_wrapper(const std::string& sceneGraphFilename,
                               const std::string& sourceFilename,
                               const Uuid<ResourceGroup>* uuid,
                               SceneGraphImportSettings* settings)
{
  convertSceneGraph(QString::fromStdString(sceneGraphFilename),
                    QString::fromStdString(sourceFilename),
                    *uuid,
                    *settings);
}

// --------------

ResourceIndex::ResourceIndex()
{
}

void ResourceIndex::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("ResourceIndex", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void loadIndex(const string &in filename)", AngelScript::asMETHOD(ResourceIndex,loadIndex), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void loadSubdirectory(const string &in filename)", AngelScript::asMETHOD(ResourceIndex,loadIndexedDirectory), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerAsset(const Uuid<StaticMesh> &in uuid, const string &in mesh_file)", AngelScript::asMETHOD(ResourceIndex,registerAsset), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  SceneGraphImportSettings::registerType();

  r = angelScriptEngine->RegisterGlobalFunction("void convertStaticMesh(const string &in meshFile, const string &in sourceFile)", AngelScript::asFUNCTION(convertStaticMesh), AngelScript::asCALL_CDECL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterGlobalFunction("void convertSceneGraph(const string &in sceneGraphFile, const string &in sourceFile, const Uuid<ResourceGroup> &in uuid, const SceneGraphImportSettings@ settings)", AngelScript::asFUNCTION(convertSceneGraph_wrapper), AngelScript::asCALL_CDECL); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void ResourceIndex::loadIndex(const std::string& filename)
{
  AngelScriptIntegration::ConfigCallScript config;
  config.accessMask = ACCESS_MASK_RESOURCE_LOADING;

  AngelScriptIntegration::callScriptExt<void>(angelScriptEngine, filename.c_str(), "void main(ResourceIndex@ index)", "resource-index", config, this);
}

void ResourceIndex::loadIndexedDirectory(const std::string& dir)
{
  loadIndex(dir+"/asset-index");
}

State ResourceIndex::stateOf(const QUuid& uuid) const
{
  if(loadedRessources.contains(uuid))
    return State::LOADED;
  else if(loadingRessources.contains(uuid))
    return State::LOADING;
  else if(unloadedRessources.contains(uuid))
    return State::REGISTERED;
  else
    return State::NONE;
}

bool ResourceIndex::isRegistered(const QUuid& uuid) const
{
  return unloadedRessources.contains(uuid)
      || isLoading(uuid)
      || isLoaded(uuid);
}

bool ResourceIndex::isLoading(const QUuid& uuid) const
{
  return loadingRessources.contains(uuid);
}

bool ResourceIndex::isLoaded(const QUuid& uuid) const
{
  return loadedRessources.contains(uuid);
}

void ResourceIndex::registerAsset(const Uuid<StaticMeshData>& uuid, const std::string& mesh_file)
{
  unloadedRessources.insert(uuid);
  staticMeshAssetsFiles[uuid] = QDir::current().absoluteFilePath(QString::fromStdString(mesh_file));
}

void ResourceIndex::_loadResource(ResourceLoader* loader, const QUuid& uuid, bool loadNow)
{
  Uuid<StaticMeshData> staticMeshUuid(uuid);

  // #TODO managage state changes

  if(staticMeshAssetsFiles.contains(staticMeshUuid))
  {
    loader->loadStaticMesh(staticMeshUuid, staticMeshAssetsFiles[staticMeshUuid].toStdString());
    if(loadNow)
      waitForAssetToBeLoaded(uuid);
  }else
  {
    qCritical() << "Trying to load resource with unregistered uuid " << uuid;
  }
}

void ResourceIndex::waitForAssetToBeLoaded(const QUuid& uuid)
{
  while(!isLoaded(uuid))
    QThread::currentThread()->msleep(5);
}

bool ResourceIndex::classInvariant()
{
  bool everyRessourceHasOnlyOneState = (unloadedRessources & loadingRessources).isEmpty()
                                    && (loadedRessources   & loadingRessources).isEmpty()
                                    && (loadedRessources   & unloadedRessources).isEmpty();

  return everyRessourceHasOnlyOneState;
}


} // namespace resources
} // namespace glrt
} // namespace scene
