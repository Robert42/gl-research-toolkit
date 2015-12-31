#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


StaticMeshComponent::StaticMeshComponent(Entity& entity,
                                         bool movable,
                                         const resources::StaticMeshUuid& staticMesh,
                                         const resources::MaterialDataUuid& material,
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

