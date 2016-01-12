#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


StaticMeshComponent::StaticMeshComponent(Node& entity,
                                         const Uuid<StaticMeshComponent>& uuid,
                                         bool isMovable,
                                         const Uuid<resources::StaticMeshData>& staticMesh,
                                         const Uuid<resources::Material>& material)
  : Node::Component(entity, uuid, isMovable),
    staticMesh(staticMesh),
    material(material)
{
}


StaticMeshComponent::~StaticMeshComponent()
{
}


} // namespace scene
} // namespace glrt

