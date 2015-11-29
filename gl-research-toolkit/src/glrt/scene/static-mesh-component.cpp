#include <glrt/scene/static-mesh-component.h>

namespace glrt {
namespace scene {


StaticMeshComponent::StaticMeshComponent(Entity& entity, StaticMesh&& staticMesh)
  : VisibleComponent(entity),
    staticMesh(std::move(staticMesh))
{
}


} // namespace scene
} // namespace glrt

