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
  const StaticMesh::Ptr staticMesh;
  const Material::Ptr material;

  StaticMeshComponent(Entity& entity, const StaticMesh::Ptr& staticMesh, const Material::Ptr& material);
  ~StaticMeshComponent();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_STATICMESHCOMPONENT_H
