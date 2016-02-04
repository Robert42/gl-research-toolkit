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

  void componentAdded(Node::Component* node);
  void componentRemoved(Node::Component* node);

  void componentShown(Node::Component* node);
  void componentHidden(Node::Component* node);

  void staticMeshComponentAdded(StaticMeshComponent* node);
  void staticMeshComponentRemoved(StaticMeshComponent* node);

private:
  friend class Node;
  friend class Scene;
  QHash<Uuid<Node>, Node*> _nodes;

  SceneLayer(const Uuid<SceneLayer>& uuid, Scene& scene);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENELAYER_H
