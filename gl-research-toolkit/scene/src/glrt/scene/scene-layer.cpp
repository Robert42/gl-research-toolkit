#include <glrt/scene/scene-layer.h>

namespace glrt {
namespace scene {

/*!
\warning The scene takes ownership over the layer
 */
SceneLayer::SceneLayer(const Uuid<SceneLayer>& uuid, Scene& scene)
  : uuid(uuid),
    scene(scene)
{
  if(scene._layers.contains(uuid))
    throw GLRT_EXCEPTION("Same uuid used twice");

  scene._layers[uuid] = this;
}

SceneLayer::~SceneLayer()
{
  scene._layers.remove(uuid);
}

QList<Node*> SceneLayer::allNodes() const
{
  return _nodes.values();
}


} // namespace scene
} // namespace glrt

