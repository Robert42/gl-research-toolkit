#include <glrt/scene/scene.h>
#include <glrt/scene/scene-layer.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/camera-component.h>
#include <glrt/scene/voxel-data-component.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/collect-scene-data.h>
#include <glrt/scene/fps-debug-controller.h>
#include <glrt/scene/scene-data.h>
#include <glrt/toolkit/assimp-glm-converter.h>

#include <QFile>
#include <QDirIterator>
#include <QCoreApplication>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {

using AngelScriptIntegration::AngelScriptCheck;


resources::ResourceManager* get_resourceManager(Scene* scene)
{
  return &scene->resourceManager;
}


// ======== Scene ==============================================================

/*!
\warning The resourceManager instance must live longer than this scene instance
*/
Scene::Scene(resources::ResourceManager* resourceManager)
  : resourceManager(*resourceManager),
    data(new Data)
{
}

Scene::~Scene()
{
  clear();

  delete data;
}

void Scene::clear()
{
  this->aabb = AABB::invalid();
  this->uuid = Uuid<Scene>();

  // the destructor of a scene layers removes the layer from the _layers hash.
  // So the list of all layers have to be copied in order to be able to go with
  // a foreach loop over it.
  QList<SceneLayer*> layers = _layers.values();
  _layers.clear();
  for(SceneLayer* l : layers)
    delete l;

  _cameras.clear();
  _lights.clear();

  sceneCleared();
}


bool Scene::handleEvents(const SDL_Event& event)
{
  return inputManager.handleEvent(event);
}


void Scene::update(float deltaTime)
{
  globalCoordUpdater.updateCoordinates();

  inputManager.update();
}


QList<SceneLayer*> Scene::allLayers()
{
  return _layers.values();
}


void Scene::load(const Uuid<Scene>& scene)
{
  SPLASHSCREEN_MESSAGE("Loading Scene");
  clear();

  this->uuid = scene;

  std::string filename = this->resourceManager.sceneFileForUuid(scene).toStdString();

  if(filename.empty())
    throw GLRT_EXCEPTION(QString("Trying to load the scene %0 which has no filename registered to it.").arg(scene.toString()));

  AngelScriptIntegration::ConfigCallScript config;
  config.accessMask = ACCESS_MASK_RESOURCE_LOADING | AngelScriptIntegration::ACCESS_MASK_GLM;

  AngelScriptIntegration::errorCount = 0;
  AngelScriptIntegration::callScriptExt<void>(angelScriptEngine, filename.c_str(), "void main(Scene@ scene)", "scene-file", config, this);

  angelScriptEngine->GarbageCollect();

  qApp->processEvents();

  bool success = AngelScriptIntegration::errorCount==0;
  sceneLoadedExt(this, success);
  sceneLoaded(success);

  qApp->processEvents();

  unloadUnusedResources();
}

void Scene::loadSceneLayer(const Uuid<SceneLayer>& sceneLayerUuid)
{
  SPLASHSCREEN_MESSAGE("Loading Scene-Layer");

  std::string filename = this->resourceManager.sceneLayerFileForUuid(sceneLayerUuid).toStdString();

  AngelScriptIntegration::ConfigCallScript config;
  config.accessMask = ACCESS_MASK_RESOURCE_LOADING | AngelScriptIntegration::ACCESS_MASK_GLM;

  SceneLayer* sceneLayer = new SceneLayer(sceneLayerUuid, *this);

  AngelScriptIntegration::callScriptExt<void>(angelScriptEngine, filename.c_str(), "void main(SceneLayer@ sceneLayer)", "scene-layer-file", config, sceneLayer);
}

void Scene::addSceneLayer_debugCamera()
{
  SceneLayer* sceneLayer = new SceneLayer(uuids::debugCameraLayer, *this);

  Node* node = new Node(*sceneLayer, Uuid<Node>(QUuid::createUuidV5(uuids::debugCameraComponent, QString("glrt::scene::Node"))));
  CameraComponent* debugCameraComponent = new CameraComponent(*node, nullptr, uuids::debugCameraComponent, CameraParameter());

  FpsDebugController* fpsController = new FpsDebugController(*debugCameraComponent, Uuid<FpsDebugController>(QUuid::createUuidV5(uuids::debugCameraComponent, QString("glrt::scene::FpsDebugController"))));
  Q_UNUSED(fpsController);
}

void Scene::set_camera(CameraSlot slot, const Uuid<CameraComponent>& uuid)
{
  _cameras[slot] = uuid;
}

Uuid<CameraComponent> Scene::camera(CameraSlot slot) const
{
  return _cameras[slot];
}


void Scene::set_light(LightSlot slot, const Uuid<resources::LightSource>& uuid)
{
  _lights[slot] = uuid;
}

Uuid<resources::LightSource> Scene::light(LightSlot slot) const
{
  return _lights[slot];
}

void Scene::registerAngelScriptAPIDeclarations()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("Scene", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);

  glrt::Uuid<void>::registerCustomizedUuidType("Scene", false);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void Scene::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterEnum("CameraSlot"); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("CameraSlot", "MAIN_CAMERA", static_cast<int>(CameraSlot::MAIN_CAMERA)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterEnum("LightSlot"); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterEnumValue("LightSlot", "MAIN_SUN", static_cast<int>(LightSlot::MAIN_SUN)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterObjectMethod("Scene", "void set_camera(CameraSlot slot, const Uuid<CameraComponent> &in uuid)", AngelScript::asMETHOD(Scene,set_camera), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("Scene", "Uuid<CameraComponent> get_camera(CameraSlot slot)", AngelScript::asMETHOD(Scene,camera), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("Scene", "void set_light(LightSlot slot, const Uuid<LightSource> &in uuid)", AngelScript::asMETHOD(Scene,set_camera), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("Scene", "Uuid<LightSource> get_light(LightSlot slot)", AngelScript::asMETHOD(Scene,camera), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("Scene", "void loadSceneLayer(const Uuid<SceneLayer> &in uuid)", AngelScript::asMETHOD(Scene,loadSceneLayer), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("Scene", "void addSceneLayer_debugCamera()", AngelScript::asMETHOD(Scene,addSceneLayer_debugCamera), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("Scene", "ResourceManager@ get_resourceManager()", AngelScript::asFUNCTION(get_resourceManager), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void Scene::unloadUnusedResources()
{
  using resources::StaticMesh;
  using resources::StaticMeshLoader;
  using resources::Texture;
  using resources::TextureManager;
  using resources::Material;

  TextureManager& textureManager = resourceManager.textureManager;
  StaticMeshLoader& staticMeshLoader = resourceManager.staticMeshLoader;

  QSet<Uuid<StaticMesh>> allUsedStaticMeshes;
  QSet<Uuid<Texture>> allUsedTextures;
  Array<StaticMeshComponent*> allStaticMeshComponents = collectAllComponentsWithType<StaticMeshComponent>(this);

  for(const StaticMeshComponent* staticMeshComponent : allStaticMeshComponents)
  {
    allUsedStaticMeshes.insert(staticMeshComponent->staticMeshUuid);

    Material material = staticMeshComponent->material();

    if(material.isTextureType())
    {
      switch(material.textureHandleType)
      {
      case Material::TextureHandleType::Ids:
        allUsedTextures.insert(textureManager.textureUuidForHandle(material.texturesIds.basecolor_map));
        allUsedTextures.insert(textureManager.textureUuidForHandle(material.texturesIds.normal_map));
        allUsedTextures.insert(textureManager.textureUuidForHandle(material.texturesIds.height_map));
        allUsedTextures.insert(textureManager.textureUuidForHandle(material.texturesIds.srmo_map));
        allUsedTextures.insert(textureManager.textureUuidForHandle(material.texturesIds.emission_map));
        break;
      case Material::TextureHandleType::GpuPtrs:
        allUsedTextures.insert(textureManager.textureUuidForGpuPtr(material.textureGpuPtrs.basecolor_map));
        allUsedTextures.insert(textureManager.textureUuidForGpuPtr(material.textureGpuPtrs.normal_map));
        allUsedTextures.insert(textureManager.textureUuidForGpuPtr(material.textureGpuPtrs.height_map));
        allUsedTextures.insert(textureManager.textureUuidForGpuPtr(material.textureGpuPtrs.srmo_map));
        allUsedTextures.insert(textureManager.textureUuidForGpuPtr(material.textureGpuPtrs.emission_map));
        break;
      }
    }
  }

  textureManager.removeUnusedTextures(allUsedTextures);
  staticMeshLoader.removeUnusedStaticMeshes(allUsedStaticMeshes);
}

} // namespace scene
} // namespace glrt

