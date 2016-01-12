#include <glrt/scene/scene.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/camera-component.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/toolkit/assimp-glm-converter.h>

#include <QFile>
#include <QDirIterator>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace glrt {
namespace scene {

using AngelScriptIntegration::AngelScriptCheck;


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
  clearScene();

  sceneCleared();
}


bool Scene::handleEvents(const SDL_Event& event)
{
  return debugCamera.handleEvents(event);
}


void Scene::update(float deltaTime)
{
  debugCamera.update(deltaTime);
}


QList<SceneLayer*> Scene::allLayers()
{
  return _layers.values();
}


void Scene::load(const Uuid<Scene>& scene)
{
  clear();
  // #TODO
  Q_UNUSED(scene);
}

void Scene::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("Scene", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);

  glrt::Uuid<void>::registerCustomizedUuidType("Scene", false);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace scene
} // namespace glrt

