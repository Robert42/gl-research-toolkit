#include <glrt/scene/scene.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/camera-component.h>
#include <glrt/toolkit/assimp-glm-converter.h>
#include <glrt/toolkit/json.h>

#include <glrt/glsl/layout-constants.h>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <QDirIterator>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace glrt {
namespace scene {


// ======== Scene ==============================================================


Scene::Scene(SDL_Window* sdlWindow)
  : debugCamera(sdlWindow),
    _cachedStaticStructureCacheIndex(0)
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
  showSplashscreenMessage("search Scenes");

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


bool Scene::loadFromFile(const QString& filename)
{
  clear();

  QJsonDocument jsonDocument = readJsonFile(filename);

  bool success = fromJson(QFileInfo(filename).dir(), jsonDocument.object());

  sceneLoadedExt(this, success);
  sceneLoaded(success);

  return success;
}

bool Scene::fromJson(const QDir& dir, const QJsonObject& json)
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

  if(!QFileInfo(this->file).exists())
    this->file = dir.filePath(this->file);

  SceneAssets assets;
  assets.fallbackMaterial = MaterialInstance::createDummyMaterial();

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

      MaterialInstance::Ptr material = MaterialInstance::fromJson(dir, value.toObject());

      if(material.isNull())
        return false;

      assets.materials[material->name] = material;
    }
  }

  if(json.contains("lights"))
  {
    if(!json["lights"].isArray())
    {
      qWarning() << "Scene::loadFromJson: lights must be an array";
      return false;
    }

    for(const QJsonValue& value : json["lights"].toArray())
    {
      if(!value.isObject())
      {
        qWarning() << "Scene::loadFromJson: lights must be an array of objects";
        return false;
      }

      QJsonObject lightObject = value.toObject();

      if(!lightObject.contains("type") || !lightObject["type"].isString())
      {
        qWarning() << "Scene::loadFromJson: a light contain a type";
        return false;
      }

      if(!lightObject.contains("name") || !lightObject["name"].isString())
      {
        qWarning() << "Scene::loadFromJson: a light contain a name";
        return false;
      }

      QString type = lightObject["type"].toString();
      QString name = lightObject["name"].toString();

      if(type == "sphere-area-light")
      {
        SphereAreaLightComponent::Data data;
        if(!data.initFromJson(lightObject))
          return false;

        assets.sphereAreaLights[name] = data;
      }else if(type == "rect-area-light")
      {
        RectAreaLightComponent::Data data;
        if(!data.initFromJson(lightObject))
          return false;

        assets.rectAreaLights[name] = data;
      }else
      {
        qWarning() << "Scene::loadFromJson: Unknown light type " << type;
        return false;
      }
    }
  }

  bool success = loadFromColladaFile(this->file, assets);

  if(json.contains("debug-camera"))
  {
    if(!json["debug-camera"].isObject())
    {
      qWarning() << "Scene::loadFromJson: debug-camera mus be an object";
      return false;
    }
    QJsonObject jsonDebugCamera = json["debug-camera"].toObject();
    if(!this->debugCamera.fromJson(jsonDebugCamera, this->sceneCameras()))
      return false;
  }else
  {
    debugCamera.loadedName = "";
  }

  return success;
}


bool Scene::loadFromColladaFile(const QString& file,
                                SceneAssets assets)
{
  const bool indexed = true;
  Assimp::Importer importer;

  glm::mat3 meshRotation = glm::mat3(1, 0, 0,
                                     0, 0, 1,
                                     0,-1, 0);

  glm::mat4 globalTransform = glm::mat4(1, 0, 0, 0,
                                        0, 0, 1, 0,
                                        0,-1, 0, 0,
                                        0, 0, 0, 1);

  glm::mat4 meshTransform = glm::mat4(1, 0, 0, 0,
                                      0, 0,-1, 0,
                                      0, 1, 0, 0,
                                      0, 0, 0, 1);

  const aiScene* scene = importer.ReadFile(file.toStdString(),
                                           (indexed ? aiProcess_JoinIdenticalVertices : 0) | // Use Index Buffer
                                           aiProcess_ValidateDataStructure |
                                           aiProcess_ImproveCacheLocality |
                                           aiProcess_FindDegenerates  |
                                           aiProcess_FindInvalidData  |
                                           aiProcess_CalcTangentSpace | // If there are no tangents, generate them
                                           aiProcess_GenNormals | // If there are no normals, generate them
                                           aiProcess_GenUVCoords  | // If there are no UVs auto generate some replacement
                                           aiProcess_SortByPType  | // splits meshes with multiple primitive types into multiple meshes. This way we don't have to check, face is a line or a point
                                           aiProcess_Triangulate // Triangulare quads into triangles
                                           );


  assets.scene = scene;
  assets.meshTransform = meshTransform;

  if(!scene)
    throw GLRT_EXCEPTION(QString("Couldn't load scene: %0").arg(importer.GetErrorString()));

  for(quint32 i=0; i<scene->mNumMaterials; ++i)
  {
    aiString name;
    if(scene->mMaterials[i]->Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS)
    {
      QString n = name.C_Str();
      if(assets.materials.contains(n) || assets.materials.contains(n.remove("-material")))
        assets.materialsForIndex[i] = assets.materials[n];
    }
  }

  for(quint32 i=0; i<scene->mNumCameras; ++i)
    assets.cameras[scene->mCameras[i]->mName.C_Str()] = CameraParameter::fromAssimp(*scene->mCameras[i]);

  for(quint32 i=0; i<scene->mNumMeshes; ++i)
  {
    aiMesh* mesh = scene->mMeshes[i];
    QString name = mesh->mName.C_Str();

    StaticMesh::Ptr staticMesh(new StaticMesh(std::move(StaticMesh::loadMeshFromAssimp(&mesh, 1, meshRotation, QString("\n in mesh %1 (%2) of the scene file <%0>").arg(file).arg(i).arg(name), indexed))));

    assets.meshes[name] = staticMesh;
    assets.meshesForIndex[i] = staticMesh;
  }

  return loadEntitiesFromAssimp(assets, scene->mRootNode, globalTransform);
}


bool Scene::loadEntitiesFromAssimp(const SceneAssets& assets,
                                   aiNode* node,
                                   glm::mat4 globalTransform)
{
  globalTransform = globalTransform * to_glm_mat4(node->mTransformation);

  const QString name = node->mName.C_Str();

  const bool hasMesh = node->mNumMeshes > 0;
  const bool hasCamera = assets.cameras.contains(name);
  const bool hasSphereAreaLight = assets.sphereAreaLights.contains(name);
  const bool hasRectAreaLight = assets.rectAreaLights.contains(name);
  const bool hasLight = hasSphereAreaLight || hasRectAreaLight;

  if(hasMesh || hasCamera || hasLight)
  {
    Entity* entity = new Entity(*this);
    entity->name = name;
    entity->relativeTransform = globalTransform;

    for(quint32 i=0; i<node->mNumMeshes; ++i)
    {
      quint32 meshIndex = node->mMeshes[i];

      Q_ASSERT(assets.meshesForIndex.contains(meshIndex));
      Q_ASSERT(meshIndex < assets.scene->mNumMeshes);

      StaticMesh::Ptr mesh = assets.meshesForIndex[meshIndex];
      MaterialInstance::Ptr material;

      int materialIndex = assets.scene->mMeshes[meshIndex]->mMaterialIndex;

      if(assets.materialsForIndex.contains(materialIndex))
        material = assets.materialsForIndex[materialIndex];
      else
        material = assets.fallbackMaterial;

      new StaticMeshComponent(*entity, false, mesh, material, assets.meshTransform);
    }

    if(hasCamera)
    {
      new CameraComponent(*entity, assets.cameras[name]);
    }

    if(hasSphereAreaLight)
    {
      new SphereAreaLightComponent(*entity, assets.sphereAreaLights[name]);
    }

    if(hasRectAreaLight)
    {
      new RectAreaLightComponent(*entity, assets.rectAreaLights[name]);
    }
  }

  for(quint32 i=0; i<node->mNumChildren; ++i)
  {
    if(!loadEntitiesFromAssimp(assets, node->mChildren[i], globalTransform))
      return false;
  }

  return true;
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

