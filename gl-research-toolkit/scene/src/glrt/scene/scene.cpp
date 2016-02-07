#include <glrt/scene/scene.h>
#include <glrt/scene/scene-layer.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/camera-component.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/collect-scene-data.h>
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
  : resourceManager(*resourceManager)
{
}

Scene::~Scene()
{
  clear();
}

void Scene::clear()
{
  this->uuid = Uuid<Scene>();

  // the destructor of a scene layers removes the layer from the _layers hash.
  // So the list of all layers have to be copied in order to be able to go with
  // a foreach loop over it.
  QList<SceneLayer*> layers = _layers.values();
  _layers.clear();
  for(SceneLayer* l : layers)
    delete l;

  sceneCleared();
}


bool Scene::handleEvents(const SDL_Event& event)
{
  return debugCamera.handleEvents(event);
}


void Scene::update(float deltaTime)
{
  tickManager.tick(deltaTime);
  globalCoordUpdater.updateCoordinages();
  debugCamera.update(deltaTime);
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

  AngelScriptIntegration::ConfigCallScript config;
  config.accessMask = ACCESS_MASK_RESOURCE_LOADING | AngelScriptIntegration::ACCESS_MASK_GLM;

  AngelScriptIntegration::callScriptExt<void>(angelScriptEngine, filename.c_str(), "void main(Scene@ scene)", "scene-file", config, this);

  angelScriptEngine->GarbageCollect();

  qApp->processEvents();

  // #TODO camera handling shouldn't be done by the scene
  QVector<CameraParameter> cameras = collectCameras(this);
  if(!cameras.isEmpty())
    this->debugCamera = cameras.first();

  bool success = true; // #FIXME: really find out, whether this was a success
  sceneLoadedExt(this, success);
  sceneLoaded(success);

  qApp->processEvents();
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

  r = angelScriptEngine->RegisterObjectMethod("Scene", "void loadSceneLayer(const Uuid<SceneLayer> &in uuid)", AngelScript::asMETHOD(Scene,loadSceneLayer), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("Scene", "ResourceManager@ get_resourceManager()", AngelScript::asFUNCTION(get_resourceManager), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace scene
} // namespace glrt

