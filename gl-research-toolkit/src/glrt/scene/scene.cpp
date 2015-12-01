#include <glrt/scene/scene.h>
#include <glrt/scene/static-mesh-component.h>

#include <glrt/glsl/layout-constants.h>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>

namespace glrt {
namespace scene {


// ======== Scene ==============================================================


Scene::Scene(SDL_Window* sdlWindow)
  : camera(sdlWindow),
    _cachedStaticStructureCacheIndex(0)
{
}

Scene::~Scene()
{
  QSet<Entity*> entities;
  entities.swap(this->_entities);

  for(Entity* entity : entities)
    delete entity;
}


bool Scene::handleEvents(const SDL_Event& event)
{
  return camera.handleEvents(event);
}


void Scene::update(float deltaTime)
{
  camera.update(deltaTime);
}


bool Scene::loadFromFile(const QString& filename)
{
  QFile file(filename);

  if(!file.open(QFile::ReadOnly))
  {
    qWarning() << "Scene::loadFromFile: Couldn't open file <" << filename.toStdString().c_str() << ">";
    return false;
  }

  QJsonParseError error;

  QByteArray json = file.readAll()  ;
  QJsonDocument jsonDocument = QJsonDocument::fromJson(json, &error);

  if(error.error != QJsonParseError::NoError)
  {
    qWarning() << "Scene json parsing error:\n" << error.errorString().toStdString().c_str() << "\n<" << filename.toStdString().c_str() << ">  - offset: " << error.offset;
    return false;
  }

  return fromJson(jsonDocument.object());
}

bool Scene::fromJson(const QJsonObject& json)
{
  if(!json.contains("name"))
  {
    qWarning() << "Scene::loadFromJson: Missing name";
    return false;
  }
  if(!json.contains("file"))
  {
    qWarning() << "Scene::loadFromJson: Missing file";
    return false;
  }

  this->name = json["name"].toString();
  this->file = json["file"].toString();

  QHash<QString, MaterialInstance::Ptr> materials;

  if(json.contains("materials"))
  {
    if(!json["materials"].isArray())
    {
      qWarning() << "Scene::loadFromJson: materials must be an array";
      return false;
    }

    for(const QJsonValue& value : json["materials"].toArray())
    {
      if(!value.isObject())
      {
        qWarning() << "Scene::loadFromJson: materials must be an array of objects";
        return false;
      }

      MaterialInstance::Ptr material = MaterialInstance::fromJson(value.toObject());

      if(material.isNull())
        return false;

      materials[material->name] = material;
    }
  }
  return true;
}


void Scene::staticMeshStructureChanged()
{
  ++_cachedStaticStructureCacheIndex;
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

