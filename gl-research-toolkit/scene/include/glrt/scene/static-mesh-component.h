#ifndef GLRT_SCENE_STATICMESHCOMPONENT_H
#define GLRT_SCENE_STATICMESHCOMPONENT_H

#include "entity.h"
#include "resources/resource-index.h"
#include "material.h"

namespace glrt {
namespace scene {

class StaticMeshComponent final : public VisibleComponent
{
  Q_OBJECT
public:
  const resources::StaticMeshUuid staticMesh;
  const MaterialInstance::Ptr materialInstance;

  StaticMeshComponent(Entity& entity, bool movable, const resources::StaticMeshUuid& staticMesh, const MaterialInstance::Ptr& materialInstance, const glm::mat4& relativeTransform=glm::mat4(1));
  ~StaticMeshComponent();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_STATICMESHCOMPONENT_H
