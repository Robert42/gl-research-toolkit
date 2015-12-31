#ifndef GLRT_SCENE_STATICMESHCOMPONENT_H
#define GLRT_SCENE_STATICMESHCOMPONENT_H

#include "entity.h"

namespace glrt {
namespace scene {

class StaticMeshComponent final : public VisibleComponent
{
  Q_OBJECT
public:
  const Uuid<resources::StaticMeshData> staticMesh;
  const Uuid<resources::MaterialData> material;

  StaticMeshComponent(Entity& entity, bool movable, const Uuid<resources::StaticMeshData>& staticMesh, const Uuid<resources::MaterialData> & material, const glm::mat4& relativeTransform=glm::mat4(1));
  ~StaticMeshComponent();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_STATICMESHCOMPONENT_H
