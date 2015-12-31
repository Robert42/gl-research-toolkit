#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


StaticMeshComponent::StaticMeshComponent(Entity& entity,
                                         bool movable,
                                         const Uuid<resources::StaticMeshData>& staticMesh,
                                         const Uuid<resources::MaterialData>& material,
                                         const glm::mat4& relativeTransform)
  : VisibleComponent(entity, movable, relativeTransform),
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

