#ifndef GLRT_SCENE_SCENELAYER_H
#define GLRT_SCENE_SCENELAYER_H

#include "scene.h"

namespace glrt {
namespace scene {

class SceneLayer final
{
public:
  const Uuid<SceneLayer> uuid;
  Scene& scene;

  SceneLayer(const Uuid<SceneLayer>& uuid, Scene& scene);
  ~SceneLayer();

  SceneLayer(const SceneLayer&) = delete;
  SceneLayer(SceneLayer&&) = delete;

  QList<Node*> allNodes() const;

private:
  friend class Node;
  QHash<Uuid<Node>, Node*> _nodes;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENELAYER_H
