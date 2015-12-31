#include <glrt/scene/scene.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/camera-component.h>
#include <glrt/toolkit/assimp-glm-converter.h>
#include <glrt/toolkit/json.h>

#include <QFile>
#include <QDirIterator>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace glrt {
namespace scene {


// ======== Scene ==============================================================


Scene::Scene()
  : _cachedStaticStructureCacheIndex(0)
{
}

Scene::~Scene()
{
  clear();
}

void Scene::clear()
{
  clearScene();

  QSet<Entity*> entities;
  entities.swap(this->_entities);

  for(Entity* entity : entities)
    delete entity;

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


QMap<QString, QString> Scene::findAllScenes()
{
  SPLASHSCREEN_MESSAGE("search Scenes");

  QMap<QString, QString> map;

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

  return map;
}


void Scene::loadFromFile(const QString& filename)
{
  // #TODO
  Q_UNUSED(filename);
}


void Scene::staticMeshStructureChanged()
{
  ++_cachedStaticStructureCacheIndex;
}


QMap<QString, CameraParameter> Scene::sceneCameras() const
{
  QMap<QString, CameraParameter> cameras;

  QVector<CameraComponent*> cameraComponents = allComponentsWithType<CameraComponent>();

  for(CameraComponent* c : cameraComponents)
  {
    cameras[c->entity.name] = c->globalTransformation() * c->cameraParameter;
  }

  return cameras;
}


QMap<QString, SphereAreaLightComponent::Data> Scene::sphereAreaLights() const
{
  QMap<QString, SphereAreaLightComponent::Data> lights;

  QVector<SphereAreaLightComponent*> lightComponent = allComponentsWithType<SphereAreaLightComponent>();

  for(SphereAreaLightComponent* c : lightComponent)
  {
    lights[c->entity.name] = c->globalTransformation() * c->data;
  }

  return lights;
}


QMap<QString, RectAreaLightComponent::Data> Scene::rectAreaLights() const
{
  QMap<QString, RectAreaLightComponent::Data> lights;

  QVector<RectAreaLightComponent*> lightComponent = allComponentsWithType<RectAreaLightComponent>();

  for(RectAreaLightComponent* c : lightComponent)
  {
    lights[c->entity.name] = c->globalTransformation() * c->data;
  }

  return lights;
}


void Scene::AddEntity(Entity* entity)
{
  _entities.insert(entity);
}

void Scene::RemoveEntity(Entity*entity)
{
  _entities.remove(entity);
}


} // namespace scene
} // namespace glrt

