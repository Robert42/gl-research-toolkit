#ifndef GLRT_SCENE_STATICMESHCOMPONENT_H
#define GLRT_SCENE_STATICMESHCOMPONENT_H

#include "node.h"

namespace glrt {
namespace scene {

class StaticMeshComponent final : public Node::Component
{
public:
  const Uuid<resources::StaticMeshData> staticMesh; // #TODO rename StaticMeshData to StaticMesh
  const Uuid<resources::Material> material;

  StaticMeshComponent(Node& entity, const Uuid<StaticMeshComponent>& uuid, bool isMovable, const Uuid<resources::StaticMeshData>& staticMesh, const Uuid<resources::Material> & material);
  ~StaticMeshComponent();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_STATICMESHCOMPONENT_H
