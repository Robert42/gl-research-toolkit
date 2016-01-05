#ifndef GLRT_SCENE_RESOURCES_SCENELOADER_H
#define GLRT_SCENE_RESOURCES_SCENELOADER_H

#include <glrt/toolkit/uuid.h>

#include <glrt/scene/scene.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/resources/resource-loader.h>

#include <QJsonObject>

namespace glrt {
namespace scene {
namespace resources {
/*
class SceneLoader final
{
public:
  Scene resultingScene;
  const aiScene* scene;
  QHash<QString, Uuid<MaterialData>> materials;
  QHash<QString, Uuid<StaticMeshData>> meshes;
  QHash<int, Uuid<MaterialData>> materialsForIndex;
  QHash<int, Uuid<StaticMeshData>> meshesForIndex;
  QMap<QString, CameraParameter> cameras;
  QMap<QString, SphereAreaLightComponent::Data> sphereAreaLights;
  QMap<QString, RectAreaLightComponent::Data> rectAreaLights;
  Uuid<MaterialData> fallbackMaterial;
  glm::mat4 meshTransform;

  SceneLoader(ResourceLoader* resourceLoader);
  ~SceneLoader();

  SceneLoader(const SceneLoader&) = delete;
  SceneLoader(SceneLoader&&) = delete;
  SceneLoader& operator=(const SceneLoader&) = delete;
  SceneLoader& operator=(SceneLoader&&) = delete;

  bool loadFromFile(const QString& filename);
  bool fromJson(const QDir& dir, const QJsonObject& json);

  bool loadFromColladaFile(const QString& file);
  bool loadEntitiesFromAssimp(aiNode* node, glm::mat4 globalTransform);

private:
  ResourceLoader* resourceLoader;
};
*/
} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_SCENELOADER_H
