#ifndef GLRT_SCENE_SCENE_H
#define GLRT_SCENE_SCENE_H

#include <glrt/dependencies.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/node.h>
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

  Scene(resources::ResourceManager* resourceManager);
  Scene(const Scene&) = delete;
  Scene(Scene&&) = delete;
  Scene& operator=(const Scene&) = delete;
  Scene& operator=(Scene&&) = delete;

  ~Scene();

  bool handleEvents(const SDL_Event& event);
  void update(float deltaTime);

  QList<SceneLayer*> allLayers();

  void clear();
  void load(const Uuid<Scene>& scene);

  void loadSceneLayer(const Uuid<SceneLayer>& sceneLayerUuid);

  static void registerAngelScriptAPI();

signals:
  void clearScene();
  void sceneCleared();
  void sceneLoadedExt(scene::Scene* scene, bool success);
  void sceneLoaded(bool success);

private:
  friend class SceneLayer;

  QHash<Uuid<SceneLayer>, SceneLayer*> _layers;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENE_H
