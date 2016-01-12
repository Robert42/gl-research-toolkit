#include <glrt/scene/resources/resource-index.h>
#include <glrt/scene/resources/asset-converter.h>
#include <glrt/scene/resources/material.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/scene-layer.h>
#include <QThread>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;

void convertSceneGraph_wrapper(const std::string& sceneGraphFilename,
                               const std::string& sourceFilename,
                               SceneGraphImportSettings::AngelScriptInterface* settings,
                               const ResourceIndex* resourceIndex)
{
  convertSceneGraph(QString::fromStdString(sceneGraphFilename),
                    QString::fromStdString(sourceFilename),
                    resourceIndex->uuid,
                    settings);
}

// --------------

ResourceIndex::ResourceIndex(const Uuid<ResourceIndex>& uuid)
  : uuid(uuid)
{
}

void ResourceIndex::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  Material::registerAngelScriptTypes();
  LightSource::registerAngelScriptTypes();
  SceneLayer::registerAngelScriptAPI();
  Scene::registerAngelScriptAPI();

  r = angelScriptEngine->RegisterObjectType("ResourceIndex", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);

  glrt::Uuid<void>::registerCustomizedUuidType("ResourceIndex", false);

  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void loadIndex(const string &in filename)", AngelScript::asMETHOD(ResourceIndex,loadIndex), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void loadSubdirectory(const string &in filename)", AngelScript::asMETHOD(ResourceIndex,loadIndexedDirectory), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerStaticMesh(const Uuid<StaticMesh> &in uuid, const string &in file)", AngelScript::asMETHOD(ResourceIndex,registerStaticMesh), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerLightSource(const Uuid<LightSource> &in uuid, const LightSource &in light)", AngelScript::asMETHOD(ResourceIndex,registerLightSource), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerMaterial(const Uuid<Material> &in uuid, const Material &in material)", AngelScript::asMETHOD(ResourceIndex,registerMaterial), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerSceneLayerFile(const Uuid<SceneLayer> &in uuid, const string &in file)", AngelScript::asMETHOD(ResourceIndex,registerSceneLayerFile), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  SceneGraphImportSettings::registerType();

  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void convertStaticMesh(const string &in meshFile, const string &in sourceFile)", AngelScript::asFUNCTION(convertStaticMesh), AngelScript::asCALL_CDECL_OBJLAST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void convertSceneGraph(const string &in sceneGraphFile, const string &in sourceFile, const SceneGraphImportSettings@ settings)", AngelScript::asFUNCTION(convertSceneGraph_wrapper), AngelScript::asCALL_CDECL_OBJLAST); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void ResourceIndex::loadIndex(const std::string& filename)
{
  AngelScriptIntegration::ConfigCallScript config;
  config.accessMask = ACCESS_MASK_RESOURCE_LOADING | AngelScriptIntegration::ACCESS_MASK_GLM;

  AngelScriptIntegration::callScriptExt<void>(angelScriptEngine, filename.c_str(), "void main(ResourceIndex@ index)", "resource-index", config, this);
}

void ResourceIndex::loadIndexedDirectory(const std::string& dir)
{
  loadIndex(dir+"/asset-index");
}

/*! \note This is one of the few methods which can be called with \c{this==nullptr}
 */
bool ResourceIndex::isRegistered(const QUuid& uuid) const
{
  if(this == nullptr)
    return false;
  return allRegisteredResources.contains(uuid);
}

/*! \note This is one of the few methods which can be called with \c{this==nullptr}
 */
QString ResourceIndex::labelForUuid(const QUuid& uuid, const QString& fallback) const
{
  if(this == nullptr)
    return fallback;
  return _labels.value(uuid, fallback);
}

QString ResourceIndex::labelForUuid(const QUuid& uuid) const
{
  return labelForUuid(uuid, uuid.toString());
}

void ResourceIndex::registerStaticMesh(const Uuid<StaticMeshData>& uuid, const std::string& mesh_file)
{
  validateNotYetRegistered(uuid);
  allRegisteredResources.insert(uuid);
  staticMeshAssetsFiles[uuid] = QDir::current().absoluteFilePath(QString::fromStdString(mesh_file));
}

void ResourceIndex::registerLightSource(const Uuid<LightSource>& uuid, const LightSource& light)
{
  validateNotYetRegistered(uuid);
  allRegisteredResources.insert(uuid);
  lightSources[uuid] = light;
}

void ResourceIndex::registerMaterial(const Uuid<Material>& uuid, const Material& material)
{
  validateNotYetRegistered(uuid);
  allRegisteredResources.insert(uuid);
  materials[uuid] = material;
}

void ResourceIndex::registerSceneLayerFile(const Uuid<SceneLayer>& uuid, const std::string& file)
{
  validateNotYetRegistered(uuid);
  allRegisteredResources.insert(uuid);
  scenelayerFiles[uuid] = QDir::current().absoluteFilePath(QString::fromStdString(file));
}

void ResourceIndex::validateNotYetRegistered(const QUuid& uuid) const
{
  if(isRegistered(uuid))
    throw GLRT_EXCEPTION(QString("The uuid %0 is already in use!").arg(uuid.toString()));
}


} // namespace resources
} // namespace glrt
} // namespace scene
