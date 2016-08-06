#include <glrt/scene/scene-data.h>

namespace glrt {
namespace scene {

Scene::Data::Data()
  : transformations{emptyNodes, sphereLightData, rectLightData, staticMeshData, voxelGridData, cameras}
{
  Q_ASSERT(transformations[quint32(DataClass::EMPTY)].position ==  emptyNodes.position);
  Q_ASSERT(transformations[quint32(DataClass::SPHERELIGHT)].position ==  sphereLightData.position);
  Q_ASSERT(transformations[quint32(DataClass::RECTLIGHT)].position ==  rectLightData.position);
  Q_ASSERT(transformations[quint32(DataClass::STATICMESH)].position ==  staticMeshData.position);
  Q_ASSERT(transformations[quint32(DataClass::VOXELGRID)].position ==  voxelGridData.position);
  Q_ASSERT(transformations[quint32(DataClass::CAMERA)].position ==  cameras.position);
}

} // namespace scene
} // namespace glrt
