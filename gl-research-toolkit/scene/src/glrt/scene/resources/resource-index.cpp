#include <glrt/scene/resources/resource-index.h>
#include <glrt/scene/resources/asset-converter.h>
#include <glrt/scene/resources/material.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/scene-layer.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/camera-component.h>
#include <glrt/scene/light-component.h>
#include <QThread>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;

void convertSceneGraph_wrapper(const std::string& sceneGraphFilename,
                               const std::string& sourceFilename,
                               SceneGraphImportSettings::AngelScriptInterface* settings,
                               const std::string& groupToImport,
                               const ResourceIndex* resourceIndex)
{
  convertSceneGraph(QString::fromStdString(sceneGraphFilename),
                    QString::fromStdString(sourceFilename),
                    resourceIndex->uuid,
                    settings);
}

inline void set_label(ResourceIndex* index, const Uuid<void>& uuid, const std::string& l)
{
  index->labels[uuid] = QString::fromStdString(l);
}

inline std::string get_label(ResourceIndex* index, const Uuid<void>& uuid)
{
  return index->labels[uuid].toStdString();
}

// --------------

const ResourceIndex ResourceIndex::fallback(uuids::fallbackIndex);

ResourceIndex::ResourceIndex(const Uuid<ResourceIndex>& uuid)
  : uuid(uuid)
{
  if(this==&fallback)
    registerFallbackIndex();
}

void ResourceIndex::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("ResourceIndex", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);

  glrt::Uuid<void>::registerCustomizedUuidType("ResourceIndex", false);

  ResourceManager::registerAngelScriptAPIDeclarations();
  SceneLayer::registerAngelScriptAPIDeclarations();
  Scene::registerAngelScriptAPIDeclarations();
  Node::registerAngelScriptAPIDeclarations();
  StaticMeshComponent::registerAngelScriptAPIDeclarations();
  CameraComponent::registerAngelScriptAPIDeclarations();
  LightComponent::registerAngelScriptAPIDeclarations();

  Material::registerAngelScriptTypes();
  LightSource::registerAngelScriptTypes();
  ResourceManager::registerAngelScriptAPI();
  SceneLayer::registerAngelScriptAPI();
  Scene::registerAngelScriptAPI();
  Node::registerAngelScriptAPI();
  StaticMeshComponent::registerAngelScriptAPI();
  CameraComponent::registerAngelScriptAPI();
  LightComponent::registerAngelScriptAPI();

  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void loadIndex(const string &in filename)", AngelScript::asMETHOD(ResourceIndex,loadIndex), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void loadSubdirectory(const string &in filename)", AngelScript::asMETHOD(ResourceIndex,loadIndexedDirectory), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerStaticMesh(const Uuid<StaticMesh> &in uuid, const string &in file)", AngelScript::asMETHOD(ResourceIndex,registerStaticMesh), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerLightSource(const Uuid<LightSource> &in uuid, const LightSource &in light)", AngelScript::asMETHOD(ResourceIndex,registerLightSource), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerMaterial(const Uuid<Material> &in uuid, const Material &in material)", AngelScript::asMETHOD(ResourceIndex,registerMaterial), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerSceneLayerFile(const Uuid<SceneLayer> &in uuid, const string &in file)", AngelScript::asMETHOD(ResourceIndex,registerSceneLayerFile), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerSceneFile(const Uuid<Scene> &in uuid, const string &in file)", AngelScript::asMETHOD(ResourceIndex,registerSceneFile), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  SceneGraphImportSettings::registerType();

  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void convertStaticMesh(const string &in meshFile, const string &in sourceFile, const string &in groupToImport=\"\")", AngelScript::asFUNCTION(convertStaticMesh), AngelScript::asCALL_CDECL_OBJLAST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void convertSceneGraph(const string &in sceneGraphFile, const string &in sourceFile, const SceneGraphImportSettings@ settings, const string &in groupToImport=\"\")", AngelScript::asFUNCTION(convertSceneGraph_wrapper), AngelScript::asCALL_CDECL_OBJLAST); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void set_label(const BaseUuid &in uuid, const string &in label)", AngelScript::asFUNCTION(set_label), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "string get_label(const BaseUuid &in uuid)", AngelScript::asFUNCTION(get_label), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void ResourceIndex::loadIndex(const std::string& filename)
{
  SPLASHSCREEN_MESSAGE("Loading Asset-Index");

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

void ResourceIndex::registerStaticMesh(const Uuid<StaticMesh>& uuid, const std::string& mesh_file)
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
  sceneLayerFiles[uuid] = QDir::current().absoluteFilePath(QString::fromStdString(file));
}

void ResourceIndex::registerSceneFile(const Uuid<Scene>& uuid, const std::string& file)
{
  validateNotYetRegistered(uuid);
  allRegisteredResources.insert(uuid);
  QString f = QString::fromStdString(file);
  labels[uuid] = QFileInfo(f).baseName();
  sceneFiles[uuid] = QDir::current().absoluteFilePath(f);
}

void ResourceIndex::validateNotYetRegistered(const QUuid& uuid) const
{
  if(isRegistered(uuid))
    throw GLRT_EXCEPTION(QString("The uuid %0 is already in use!").arg(uuid.toString()));
}

void ResourceIndex::registerFallbackIndex()
{
  Material::PlainColor fallbackMaterial;
  fallbackMaterial.base_color = glm::vec3(0);
  fallbackMaterial.emission = glm::vec3(1,0,1);
  registerMaterial(uuids::fallbackMaterial, fallbackMaterial);
  registerLightSource(uuids::fallbackLight, LightSource::SphereAreaLight());
}

} // namespace resources
} // namespace glrt
} // namespace scene
