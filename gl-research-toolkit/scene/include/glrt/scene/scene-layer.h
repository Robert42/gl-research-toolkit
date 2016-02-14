#ifndef GLRT_SCENE_SCENELAYER_H
#define GLRT_SCENE_SCENELAYER_H

#include "scene.h"
#include "resources/resource-index.h"

namespace glrt {
namespace scene {

class SceneLayer final : public QObject
{
  Q_OBJECT
public:
  const Uuid<SceneLayer> uuid;
  resources::ResourceIndex index;

  ~SceneLayer();

  SceneLayer(const SceneLayer&) = delete;
  SceneLayer(SceneLayer&&) = delete;

  Scene& scene();
  const Scene& scene()const;

  QList<Node*> allNodes() const;
  void loadSceneGraph(const std::string& filename);

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

private:
  friend class Scene;
  friend class Node;

  Scene& _scene;
  QHash<Uuid<Node>, Node*> _nodes;

  SceneLayer(const Uuid<SceneLayer>& uuid, Scene& scene);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENELAYER_H
