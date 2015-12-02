#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


StaticMeshComponent::StaticMeshComponent(Entity& entity, bool movable, const StaticMesh::Ptr& staticMesh, const MaterialInstance::Ptr& material)
  : VisibleComponent(entity, movable),
    staticMesh(staticMesh),
    materialInstance(material)
{
  entity.scene.staticMeshStructureChanged();
}


StaticMeshComponent::~StaticMeshComponent()
{
  entity.scene.staticMeshStructureChanged();
}


} // namespace scene
} // namespace glrt

