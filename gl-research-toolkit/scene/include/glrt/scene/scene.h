#ifndef GLRT_SCENE_SCENE_H
#define GLRT_SCENE_SCENE_H

#include <glrt/dependencies.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/entity.h>
#include <glrt/scene/debug-camera.h>

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
  Q_OBJECT
public:
  resources::ResourceManager& resourceManager;
  QString name, file;
  DebugCamera debugCamera; // #TODO this shouldn't be within the scene?

  Scene(const Scene&) = delete;
  Scene(Scene&&) = delete;
  Scene& operator=(const Scene&) = delete;
  Scene& operator=(Scene&&) = delete;

  ~Scene();

  QString labelForUuid(const QUuid& uuid) const;

  bool handleEvents(const SDL_Event& event);
  void update(float deltaTime);

  const QVector<Entity*>& allEntities();

  void clear();
  static QMap<QString, QString> findAllScenes();
  void loadFromFile(const QString& filepath);

signals:
  void clearScene();
  void sceneCleared();
  void sceneLoadedExt(scene::Scene* scene, bool success);
  void sceneLoaded(bool success);

private:
  friend class resources::ResourceManager;

  QVector<Entity*> _entities; // #TODO use an optimized array
  QHash<QUuid, QString> _labels;

  Scene(resources::ResourceManager& resourceManager);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENE_H
