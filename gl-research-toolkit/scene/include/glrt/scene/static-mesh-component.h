#ifndef GLRT_SCENE_STATICMESHCOMPONENT_H
#define GLRT_SCENE_STATICMESHCOMPONENT_H

#include "entity.h"

namespace glrt {
namespace scene {

class StaticMeshComponent final : public Entity::Component
{
public:
  const Uuid<resources::StaticMeshData> staticMesh; // #TODO rename StaticMeshData to StaticMesh
  const Uuid<resources::MaterialData> material; // #TODO rename MaterialData to Material

  StaticMeshComponent(const Uuid<StaticMeshComponent>& uuid, bool isMovable, const Uuid<resources::StaticMeshData>& staticMesh, const Uuid<resources::MaterialData> & material);
  ~StaticMeshComponent();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_STATICMESHCOMPONENT_H
