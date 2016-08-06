#ifndef GLRT_SCENE_SCENE_H
#define GLRT_SCENE_SCENE_H

#include <glrt/dependencies.h>
#include <glrt/scene/aabb.h>
#include <glrt/scene/declarations.h>
#include <glrt/scene/node.h>
#include <glrt/scene/global-coord-updater.h>
#include <glrt/scene/input-handler.h>
#include <glrt/scene/fps-debug-controller.h>
#include <glrt/scene/tick-manager.h>

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
  class Data;

  resources::ResourceManager& resourceManager;
  Uuid<Scene> uuid;
  QString file;
  GlobalCoordUpdater globalCoordUpdater;
  InputHandler::Manager inputManager;

  Data* const data;

  AABB aabb;
  TickManager tickManager;

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

  void set_light(LightSlot slot, const Uuid<resources::LightSource>& uuid);
  Uuid<resources::LightSource> light(LightSlot slot) const;

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

  void unloadUnusedResources();

signals:
  void sceneCleared();
  void sceneLoadedExt(scene::Scene* scene, bool success);
  void sceneLoaded(bool success);

  void sceneRerecordedCommands();

  void nodeAdded(Node* node);
  void nodeRemoved(Node* node);

  void componentAdded(Node::Component* node);
  void componentRemoved(Node::Component* node);

  void componentShown(Node::Component* node);
  void componentHidden(Node::Component* node);

  void CameraComponentAdded(CameraComponent* component);
  void LightComponentAdded(LightComponent* component);
  void SphereAreaLightComponentAdded(SphereAreaLightComponent* component);
  void RectAreaLightComponentAdded(RectAreaLightComponent* component);
  void VoxelDataComponentAdded(VoxelDataComponent* component);
  void StaticMeshComponentAdded(StaticMeshComponent* component);

private:
  friend class SceneLayer;

  QHash<Uuid<SceneLayer>, SceneLayer*> _layers;

  QMap<CameraSlot, Uuid<CameraComponent>> _cameras;
  QMap<LightSlot, Uuid<resources::LightSource>> _lights;
};


namespace implementation
{

template<class T>
struct ComponentAddedSignal;


} // namespace implementation

} // namespace scene
} // namespace glrt

#include "scene.inl"

#endif // GLRT_SCENE_SCENE_H
