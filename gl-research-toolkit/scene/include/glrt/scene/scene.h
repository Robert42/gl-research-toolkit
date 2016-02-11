#ifndef GLRT_SCENE_SCENE_H
#define GLRT_SCENE_SCENE_H

#include <glrt/dependencies.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/node.h>
#include <glrt/scene/tick-manager.h>
#include <glrt/scene/global-coord-updater.h>
#include <glrt/scene/input-handler.h>
#include <glrt/scene/fps-debug-controller.h>

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
  Uuid<Scene> uuid;
  QString file;
  TickManager tickManager;
  GlobalCoordUpdater globalCoordUpdater;
  InputHandler::Manager inputManager;

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
  void addSceneLayer_debugCamera();

  void set_camera(CameraSlot slot, const Uuid<CameraComponent>& uuid);
  Uuid<CameraComponent> camera(CameraSlot slot) const;

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

signals:
  void sceneCleared();
  void sceneLoadedExt(scene::Scene* scene, bool success);
  void sceneLoaded(bool success);

  void CameraComponentAdded(CameraComponent* component);
  void LightComponentAdded(LightComponent* component);
  void StaticMeshComponentAdded(StaticMeshComponent* component);

private:
  friend class SceneLayer;

  QHash<Uuid<SceneLayer>, SceneLayer*> _layers;

  QMap<CameraSlot, Uuid<CameraComponent>> _cameras;
};

template<class T>
struct ComponentAddedSignal;

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENE_H
