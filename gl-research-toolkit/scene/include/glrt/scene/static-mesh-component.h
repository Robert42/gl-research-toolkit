#ifndef GLRT_SCENE_STATICMESHCOMPONENT_H
#define GLRT_SCENE_STATICMESHCOMPONENT_H

#include "node.h"

namespace glrt {
namespace scene {

class StaticMeshComponent final : public Node::Component
{
public:
  const Uuid<resources::StaticMesh> staticMeshUuid;
  const Uuid<resources::Material> materialUuid;

  StaticMeshComponent(Node& node, Node::Component* parent, const Uuid<StaticMeshComponent>& uuid, const Uuid<resources::StaticMesh>& staticMeshUuid, const Uuid<resources::Material> & materialUuid);
  ~StaticMeshComponent();

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

  resources::Material material() const;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_STATICMESHCOMPONENT_H
