#ifndef GLRT_SCENE_RESOURCES_MATERIAL_INL
#define GLRT_SCENE_RESOURCES_MATERIAL_INL

#include "material.h"

namespace glrt {
namespace scene {
namespace resources {


template<typename T>
void Material::addMaterialUser(const Uuid<T>& uuid)
{
  if(this->materialUser == UuidIndex::null_index<0>())
    this->materialUser = uuid.index();
  else
    this->materialUser = UuidIndex::null_index<1>();
}

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_MATERIAL_INL
