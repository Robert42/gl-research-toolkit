#ifndef GLRT_SCENE_STATICMESHCOMPONENT_H
#define GLRT_SCENE_STATICMESHCOMPONENT_H

#include "entity.h"
#include "static-mesh.h"
#include "material.h"

namespace glrt {
namespace scene {

class StaticMeshComponent final : public VisibleComponent
{
  Q_OBJECT
public:
  StaticMesh staticMesh;
  Material::Ptr material;

  StaticMeshComponent(Entity& entity, StaticMesh&& staticMesh);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_STATICMESHCOMPONENT_H
