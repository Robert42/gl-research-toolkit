#include <glrt/scene/resources/scene-loader.h>

namespace glrt {
namespace scene {
namespace resources {
/*
SceneLoader::SceneLoader(ResourceLoader* resourceLoader)
  : fallbackMaterial(QUuid("{a8f3fb1b-1168-433b-aaf8-e24632cce156}")),
    resourceLoader(resourceLoader)
{
  // FIXME: register the fallback
}



bool SceneLoader::loadFromFile(const QString& filename)
{
  resultingScene.clear();

  QJsonDocument jsonDocument = readJsonFile(filename);


  // #FIXME
  bool success = fromJson(QFileInfo(filename).dir(), jsonDocument.object());

  resultingScene.sceneLoadedExt(&resultingScene, success);
  resultingScene.sceneLoaded(success);

  return success;

  Q_UNUSED(filename);
  return false;
}



#TODO
bool SceneLoader::fromJson(const QDir& dir, const QJsonObject& json)
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

  resultingScene.name = json["name"].toString();
  resultingScene.file = json["file"].toString();

  if(!QFileInfo(resultingScene.file).exists())
    resultingScene.file = dir.filePath(resultingScene.file);

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

      materials[material->name] = material;
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

        sphereAreaLights[name] = data;
      }else if(type == "rect-area-light")
      {
        RectAreaLightComponent::Data data;
        if(!data.initFromJson(lightObject))
          return false;

        rectAreaLights[name] = data;
      }else
      {
        qWarning() << "Scene::loadFromJson: Unknown light type " << type;
        return false;
      }
    }
  }

  bool success = loadFromColladaFile(resultingScene.file);

  if(json.contains("debug-camera"))
  {
    if(!json["debug-camera"].isObject())
    {
      qWarning() << "Scene::loadFromJson: debug-camera mus be an object";
      return false;
    }
    QJsonObject jsonDebugCamera = json["debug-camera"].toObject();
    if(!resultingScene.debugCamera.fromJson(jsonDebugCamera, resultingScene.sceneCameras()))
      return false;
  }else
  {
    resultingScene.debugCamera.loadedName = "";
  }

  return success;
}


bool SceneLoader::loadFromColladaFile(const QString& file)
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
    assets.cameras[scene->mCameras[i]->mName.C_Str()] = Camera::fromAssimp(*scene->mCameras[i]);

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


bool Scene::loadEntitiesFromAssimp(aiNode* node,
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
*/

} // namespace resources
} // namespace scene
} // namespace glrt

