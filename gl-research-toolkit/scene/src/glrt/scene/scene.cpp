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


QMap<QString, QString> Scene::findAllScenes()
{
  SPLASHSCREEN_MESSAGE("search Scenes");

  QMap<QString, QString> map;
/* #TODO
  QDirIterator dirIterator(QDir(GLRT_ASSET_DIR), QDirIterator::Subdirectories|QDirIterator::FollowSymlinks);

  while(dirIterator.hasNext())
  {
    QDir dir = dirIterator.next();

    if(dir.dirName() == ".")
      continue;

    for(const QFileInfo& fileInfo : dir.entryList({"*.scene"}, QDir::Dirs|QDir::Files|QDir::Readable, QDir::Name))
    {
      QString absoluteFilename = dir.absoluteFilePath(fileInfo.fileName());
      QJsonObject json = readJsonFile(absoluteFilename).object();

      if(!json.contains("name"))
      {
        qWarning() << "The scene " << absoluteFilename << " has no name";
        continue;
      }

      QJsonValue value = json["name"];
      QString name = value.toString();

      if(name.isEmpty())
      {
        qWarning() << "The scene " << absoluteFilename << " has an empty name";
        continue;
      }

      if(map.contains(name))
      {
        qWarning() << "The scene-name " << name << " is used twice. Once for " << map[name] << " and once for " << absoluteFilename;
        continue;
      }

      map[name] = absoluteFilename;
    }
  }
*/
  return map;
}


void Scene::loadFromFile(const QString& filename)
{
  // #TODO
  Q_UNUSED(filename);
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

