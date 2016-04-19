#include <glrt/scene/resources/resource-index.h>
#include <glrt/scene/resources/asset-converter.h>
#include <glrt/scene/resources/material.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/scene-layer.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/camera-component.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/resources/texture-sampler.h>
#include <glrt/scene/resources/texture.h>
#include <glrt/scene/resources/voxelizer.h>
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
                    settings,
                    QString::fromStdString(groupToImport));
}

void convertStaticMesh_wrapper(const std::string& meshFile,
                               const std::string& sourceFile,
                               const std::string& groupToImport,
                               const MeshImportSettings* meshImportSettings,
                               const ResourceIndex* resourceIndex)
{
  convertStaticMesh(QString::fromStdString(meshFile),
                    QString::fromStdString(sourceFile),
                    QString::fromStdString(groupToImport),
                    *meshImportSettings);
  Q_UNUSED(resourceIndex);
}

void convertTexture_wrapper(const std::string& textureFilename,
                            const std::string& sourceFilename,
                            const TextureFile::ImportSettings* settings,
                            const ResourceIndex* resourceIndex)
{
  convertTexture(QString::fromStdString(textureFilename),
                 QString::fromStdString(sourceFilename),
                 *settings);
  Q_UNUSED(resourceIndex);
}

inline void set_label(ResourceIndex* index, const Uuid<void>& uuid, const std::string& l)
{
  index->labels[uuid] = QString::fromStdString(l);
}

inline std::string get_label(ResourceIndex* index, const Uuid<void>& uuid)
{
  return index->labels[uuid].toStdString();
}

inline Voxelizer get_default_voxelizer(ResourceIndex* index)
{
  return Voxelizer(index);
}

// --------------

const ResourceIndex ResourceIndex::fallback(uuids::fallbackIndex);
ResourceIndex* ResourceIndex::_fallback = nullptr;

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

  TextureSampler::registerType();
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
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void loadAllSubdirectoriesExcept(const string &in dirname)", AngelScript::asMETHOD(ResourceIndex,loadAllSubdirectoriesExcept), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void loadSubdirectory(const string &in dirname)", AngelScript::asMETHOD(ResourceIndex,loadIndexedDirectory), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void addIncludeDirectory(const string &in filename)", AngelScript::asMETHOD(ResourceIndex,addScriptIncludeDirectory), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerStaticMesh(const Uuid<StaticMesh> &in uuid, const string &in file)", AngelScript::asMETHOD(ResourceIndex,registerStaticMesh), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerLightSource(const Uuid<LightSource> &in uuid, const LightSource &in light)", AngelScript::asMETHOD(ResourceIndex,registerLightSource), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerMaterial(const Uuid<Material> &in uuid, const Material &in material)", AngelScript::asMETHOD(ResourceIndex,registerMaterial), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerSceneLayerFile(const Uuid<SceneLayer> &in uuid, const string &in file)", AngelScript::asMETHOD(ResourceIndex,registerSceneLayerFile), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerSceneFile(const Uuid<Scene> &in uuid, const string &in file)", AngelScript::asMETHOD(ResourceIndex,registerSceneFile), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void registerTextureFile(const Uuid<Texture> &in uuid, const string &in file, const TextureSampler &in defaultSampler)", AngelScript::asMETHOD(ResourceIndex,registerTexture), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  SceneGraphImportSettings::registerType();
  MeshImportSettings::registerType();
  TextureFile::ImportSettings::registerType();
  Voxelizer::registerAngelScriptAPI();

  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void convertStaticMesh(const string &in meshFile, const string &in sourceFile, const string &in groupToImport=\"\", const MeshImportSettings &in meshImportSettings = MeshImportSettings())", AngelScript::asFUNCTION(convertStaticMesh_wrapper), AngelScript::asCALL_CDECL_OBJLAST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void convertTexture(const string &in textureFile, const string &in sourceFile, const TextureImportSettings &in settings)", AngelScript::asFUNCTION(convertTexture_wrapper), AngelScript::asCALL_CDECL_OBJLAST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void convertSceneGraph(const string &in sceneGraphFile, const string &in sourceFile, const SceneGraphImportSettings@ settings, const string &in groupToImport=\"\")", AngelScript::asFUNCTION(convertSceneGraph_wrapper), AngelScript::asCALL_CDECL_OBJLAST); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "void set_label(const BaseUuid &in uuid, const string &in label)", AngelScript::asFUNCTION(set_label), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "string get_label(const BaseUuid &in uuid)", AngelScript::asFUNCTION(get_label), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectMethod("ResourceIndex", "Voxelizer get_defaultVoxelizer()", AngelScript::asFUNCTION(get_default_voxelizer), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void ResourceIndex::loadIndex(const std::string& filename)
{
  SPLASHSCREEN_MESSAGE("Loading Asset-Index");

  AngelScriptIntegration::ConfigCallScript config;
  config.accessMask = ACCESS_MASK_RESOURCE_LOADING | AngelScriptIntegration::ACCESS_MASK_GLM;
  config.includeDirectories = this->scriptIncludeDirectories;

  AngelScriptIntegration::callScriptExt<void>(angelScriptEngine, filename.c_str(), "void main(ResourceIndex@ index)", "resource-index", config, this);

  this->scriptIncludeDirectories = config.includeDirectories;
}

void ResourceIndex::loadIndexedDirectory(const std::string& dir)
{
  loadIndex(dir+"/asset-index");
}

void ResourceIndex::loadAllSubdirectoriesExcept(const std::string& dir)
{
  for(const QFileInfo& fileInfo : QDir::current().entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot))
    if(fileInfo.fileName()!=QString::fromStdString(dir) && fileInfo.isDir() && QDir(fileInfo.filePath()).exists("asset-index"))
      loadIndexedDirectory(fileInfo.fileName().toStdString());
}

void ResourceIndex::addScriptIncludeDirectory(const std::string& d)
{
  QDir dir = QDir::current();
  dir.cd(QString::fromStdString(d));

  scriptIncludeDirectories << dir.absolutePath();
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

void ResourceIndex::registerTexture(const Uuid<Texture>& uuid, const std::string& file, const TextureSampler& textureSampler)
{
  if(uuid == uuids::fallbackDiffuseTexture && _fallback!=this)
  {
    Q_ASSERT(_fallback != nullptr);
    _fallback->registerTexture(uuid, file, textureSampler);
    return;
  }

  validateNotYetRegistered(uuid);
  allRegisteredResources.insert(uuid);
  QString f = QString::fromStdString(file);
  labels[uuid] = QFileInfo(f).baseName();
  textures[uuid].setFile(QDir::current().absoluteFilePath(f));
  defaultTextureSamplers[uuid] = textureSampler;
}

void ResourceIndex::validateNotYetRegistered(const QUuid& uuid) const
{
  if(isRegistered(uuid))
    throw GLRT_EXCEPTION(QString("The uuid %0 is already in use!").arg(uuid.toString()));
}

void ResourceIndex::registerFallbackIndex()
{
  _fallback = this;
  Material::PlainColor fallbackMaterial;
  fallbackMaterial.base_color = glm::vec3(0);
  fallbackMaterial.metal_mask = 1.f; // by having metal=1, the black coor results in absolutely no lighting at all
  fallbackMaterial.emission = glm::vec3(1,0,1);
  registerMaterial(uuids::fallbackMaterial, fallbackMaterial);
  registerLightSource(uuids::fallbackLight, LightSource::SphereAreaLight());
  textures[uuids::fallbackDiffuseTexture] = Texture();
}

} // namespace resources
} // namespace glrt
} // namespace scene
