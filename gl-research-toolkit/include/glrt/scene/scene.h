#ifndef GLRT_SCENE_SCENE_H
#define GLRT_SCENE_SCENE_H

#include <glrt/dependencies.h>
#include <glrt/scene/static-mesh.h>
#include <glrt/scene/material.h>
#include <glrt/scene/entity.h>
#include <glrt/debug-camera.h>

#include <glhelper/shaderobject.hpp>
#include <glhelper/buffer.hpp>
#include <glhelper/texture2d.hpp>

#include <QJsonObject>

namespace glrt {
namespace scene {


class StaticMeshComponent;

class Scene final : public QObject
{
  Q_OBJECT

public:
  QString name, file;
  DebugCamera camera;

  Scene(const Scene&) = delete;
  Scene(Scene&&) = delete;
  Scene& operator=(const Scene&) = delete;
  Scene& operator=(Scene&&) = delete;

  Scene(SDL_Window* sdlWindow);
  ~Scene();

  bool handleEvents(const SDL_Event& event);
  void update(float deltaTime);

  bool loadFromFile(const QString& file);
  bool fromJson(const QDir& dir, const QJsonObject& json);

  void bindSceneUniformBlock();

  void staticMeshStructureChanged();

  template<typename T>
  QVector<T*> allComponentsWithType(const std::function<bool(T*)>& filter);

private:
  friend class Renderer;
  friend class Entity;
  quint64 _cachedStaticStructureCacheIndex;

  QSet<Entity*> _entities;

  void AddEntity(Entity* entity);
  void RemoveEntity(Entity* entity);
};


template<typename T>
QVector<T*> Scene::allComponentsWithType(const std::function<bool(T*)>& filter)
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
