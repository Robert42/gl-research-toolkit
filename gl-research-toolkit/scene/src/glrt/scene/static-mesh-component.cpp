#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


StaticMeshComponent::StaticMeshComponent(Entity& entity,
                                         bool movable,
                                         const StaticMeshUuid& staticMesh,
                                         const MaterialInstance::Ptr& material,
                                         const glm::mat4& relativeTransform)
  : VisibleComponent(entity, movable, relativeTransform),
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

