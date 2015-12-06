#ifndef GLRT_SCENE_SCENE_H
#define GLRT_SCENE_SCENE_H

#include <glrt/dependencies.h>
#include <glrt/scene/static-mesh.h>
#include <glrt/scene/material.h>
#include <glrt/scene/entity.h>
#include <glrt/debug-camera.h>
#include <glrt/debugging/debug-line-visualisation.h>
#include <glrt/gui/anttweakbar.h>

#include <glhelper/shaderobject.hpp>
#include <glhelper/buffer.hpp>
#include <glhelper/texture2d.hpp>

#include <QJsonObject>

struct aiNode;
struct aiScene;

namespace glrt {
namespace scene {


class StaticMeshComponent;

class Scene final : public QObject
{
  Q_OBJECT

public:
  QString name, file;
  DebugCamera debugCamera;

  gui::TweakBarCBVar<bool> visualize_sceneCameras;

  Scene(const Scene&) = delete;
  Scene(Scene&&) = delete;
  Scene& operator=(const Scene&) = delete;
  Scene& operator=(Scene&&) = delete;

  Scene(SDL_Window* sdlWindow);
  ~Scene();

  bool handleEvents(const SDL_Event& event);
  void update(float deltaTime);

  void clear();
  bool loadFromFile(const QString& file);
  bool fromJson(const QDir& dir, const QJsonObject& json);

  void staticMeshStructureChanged();

  template<typename T>
  QVector<T*> allComponentsWithType(const std::function<bool(T*)>& filter=[](T*){return true;}) const;

  QMap<QString, CameraParameter> sceneCameras() const;

private:
  friend class Renderer;
  friend class Entity;
  quint64 _cachedStaticStructureCacheIndex;

  QSet<Entity*> _entities;

  debugging::DebugLineVisualisation::Ptr _debug_sceneCameras;

  void AddEntity(Entity* entity);
  void RemoveEntity(Entity* entity);

private:
  struct SceneAssets
  {
    const aiScene* scene;
    QHash<QString, MaterialInstance::Ptr> materials;
    QHash<QString, StaticMesh::Ptr> meshes;
    QHash<int, MaterialInstance::Ptr> materialsForIndex;
    QHash<int, StaticMesh::Ptr> meshesForIndex;
    QMap<QString, CameraParameter> cameras;
    MaterialInstance::Ptr fallbackMaterial;
    glm::mat4 meshTransform;
  };
  bool loadFromColladaFile(const QString& file,
                           SceneAssets assets);
  bool loadEntitiesFromAssimp(const SceneAssets& assets, aiNode* node, glm::mat4 globalTransform);
};


template<typename T>
QVector<T*> Scene::allComponentsWithType(const std::function<bool(T*)>& filter) const
{
  static_assert(std::is_base_of<Entity::Component, T>::value, "T must inherit from Entity::Component");

  QVector<T*> components;
  components.reserve((_entities.size()+3) / 4);

  for(Entity* e : _entities)
    for(T* component : e->allComponentsWithType<T>(filter))
      components.append(component);
  return components;
}


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENE_H
