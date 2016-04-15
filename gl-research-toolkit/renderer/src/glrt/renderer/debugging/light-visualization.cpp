#include <glrt/renderer/debugging/light-visualization.h>
#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/resources/resource-index.h>
#include <glrt/scene/resources/resource-manager.h>

namespace glrt {
namespace renderer {
namespace debugging {


glrt::scene::Node::Component* createLightDecoration(glrt::scene::LightComponent* l)
{
  // Just a note: If the Light component changes, the visualization doesn't get updated

  glrt::scene::SphereAreaLightComponent* sphereAreaLightComponent = dynamic_cast<glrt::scene::SphereAreaLightComponent*>(l);
  glrt::scene::RectAreaLightComponent* rectAreaLightComponent = dynamic_cast<glrt::scene::RectAreaLightComponent*>(l);
  glrt::scene::resources::ResourceManager& resourceManager = l->resourceManager();

  if(sphereAreaLightComponent)
  {
    return new glrt::scene::StaticMeshComponent(l->node, l, Uuid<glrt::scene::StaticMeshComponent>::create("sphere-area-light-preview"), glrt::scene::resources::uuids::unitSphereMesh, resourceManager.materialUuidForLightSource(sphereAreaLightComponent->data));
  }
  if(rectAreaLightComponent)
  {
    return new glrt::scene::StaticMeshComponent(l->node, l, Uuid<glrt::scene::StaticMeshComponent>::create("rect-area-light-preview"), glrt::scene::resources::uuids::unitRectMesh, resourceManager.materialUuidForLightSource(rectAreaLightComponent->data));
  }

  Q_UNREACHABLE();
  return nullptr;
}


} // namespace debugging
} // namespace renderer
} // namespace glrt
