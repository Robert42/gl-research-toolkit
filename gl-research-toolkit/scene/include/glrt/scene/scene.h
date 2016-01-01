#ifndef GLRT_SCENE_SCENE_H
#define GLRT_SCENE_SCENE_H

#include <glrt/dependencies.h>
#include <glrt/scene/entity.h>
#include <glrt/scene/debug-camera.h>
#include <glrt/scene/light-component.h>

struct aiNode;
struct aiScene;

namespace glrt {

namespace renderer {
class Renderer;
} // namespace renderer

namespace scene {


class StaticMeshComponent;

class Scene final : public QObject
{
  Q_OBJECT // #FIXME

public:
  QString name, file;
  DebugCamera debugCamera;

  Scene(const Scene&) = delete;
  Scene(Scene&&) = delete;
  Scene& operator=(const Scene&) = delete;
  Scene& operator=(Scene&&) = delete;

  Scene();
  ~Scene();

  bool handleEvents(const SDL_Event& event);
  void update(float deltaTime);

  void clear();
  static QMap<QString, QString> findAllScenes();
  void loadFromFile(const QString& filepath);

  template<typename T>
  QVector<T*> allComponentsWithType(const std::function<bool(T*)>& filter=[](T*){return true;}) const;
signals:
  void clearScene();
  void sceneCleared();
  void sceneLoadedExt(scene::Scene* scene, bool success);
  void sceneLoaded(bool success);

private:
  QVector<Entity*> _entities; // #TODO use an optimized array
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
