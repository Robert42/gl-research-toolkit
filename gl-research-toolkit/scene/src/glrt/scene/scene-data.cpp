#include <glrt/scene/scene-data.h>

namespace glrt {
namespace scene {

Scene::Data::Data()
{
  allTransformData.append(&sphereLightData);
  allTransformData.append(&rectLightData);
  allTransformData.append(&staticMeshData);

  allLightSourceData.append(&sphereLightData);
  allLightSourceData.append(&rectLightData);
}

} // namespace scene
} // namespace glrt
