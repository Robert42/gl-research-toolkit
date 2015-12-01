#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


StaticMeshComponent::StaticMeshComponent(Entity& entity, const StaticMesh::Ptr& staticMesh, const Material::Ptr& material)
  : VisibleComponent(entity),
    staticMesh(staticMesh),
    material(material)
{
  entity.scene.staticMeshStructureChanged();
}


StaticMeshComponent::~StaticMeshComponent()
{
  entity.scene.staticMeshStructureChanged();
}


} // namespace scene
} // namespace glrt

