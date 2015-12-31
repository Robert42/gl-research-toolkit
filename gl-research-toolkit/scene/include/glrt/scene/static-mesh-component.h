#ifndef GLRT_SCENE_STATICMESHCOMPONENT_H
#define GLRT_SCENE_STATICMESHCOMPONENT_H

#include "entity.h"
#include "resources/resource-uuid.h"

namespace glrt {
namespace scene {

class StaticMeshComponent final : public VisibleComponent
{
  Q_OBJECT
public:
  const resources::StaticMeshUuid staticMesh;
  const resources::MaterialDataUuid material;

  StaticMeshComponent(Entity& entity, bool movable, const resources::StaticMeshUuid& staticMesh, const resources::MaterialDataUuid & material, const glm::mat4& relativeTransform=glm::mat4(1));
  ~StaticMeshComponent();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_STATICMESHCOMPONENT_H
