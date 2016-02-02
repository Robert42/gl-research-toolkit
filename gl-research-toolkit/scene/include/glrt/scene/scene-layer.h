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
  Scene& scene;
  resources::ResourceIndex index;

  ~SceneLayer();

  SceneLayer(const SceneLayer&) = delete;
  SceneLayer(SceneLayer&&) = delete;

  QList<Node*> allNodes() const;
  void loadSceneGraph(const std::string& filename);

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

signals:
  void nodeAdded(Node* node);
  void nodeRemoved(Node* node);

private:
  friend class Node;
  friend class Scene;
  QHash<Uuid<Node>, Node*> _nodes;
  Array<Node*> _newNodes;

  SceneLayer(const Uuid<SceneLayer>& uuid, Scene& scene);

  void _addNode(Node* node);
  void _removeNode(Node* node);

  void update();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENELAYER_H
