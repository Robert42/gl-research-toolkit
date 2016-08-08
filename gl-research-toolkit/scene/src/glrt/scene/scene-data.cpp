#include <glrt/scene/scene-data.h>

namespace glrt {
namespace scene {

Scene::Data::Data()
  : transformations{emptyNodes, sphereLights, rectLights, staticMeshes, voxelGrids, cameras}
{
  Q_ASSERT(transformations[quint32(DataClass::EMPTY)].position ==  emptyNodes.position);
  Q_ASSERT(transformations[quint32(DataClass::SPHERELIGHT)].position ==  sphereLights.position);
  Q_ASSERT(transformations[quint32(DataClass::RECTLIGHT)].position ==  rectLights.position);
  Q_ASSERT(transformations[quint32(DataClass::STATICMESH)].position ==  staticMeshes.position);
  Q_ASSERT(transformations[quint32(DataClass::VOXELGRID)].position ==  voxelGrids.position);
  Q_ASSERT(transformations[quint32(DataClass::CAMERA)].position ==  cameras.position);
}

} // namespace scene
} // namespace glrt
