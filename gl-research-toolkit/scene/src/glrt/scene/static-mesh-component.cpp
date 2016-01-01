#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


StaticMeshComponent::StaticMeshComponent(const Uuid<StaticMeshComponent>& uuid,
                                         bool isMovable,
                                         const Uuid<resources::StaticMeshData>& staticMesh,
                                         const Uuid<resources::MaterialData>& material)
  : Entity::SpatialComponent(uuid, isMovable),
    staticMesh(staticMesh),
    material(material)
{
}


StaticMeshComponent::~StaticMeshComponent()
{
}


} // namespace scene
} // namespace glrt

