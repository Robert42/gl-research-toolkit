#ifndef GLRT_SCENE_RESOURCES_RESOURCEMANAGER_INL
#define GLRT_SCENE_RESOURCES_RESOURCEMANAGER_INL

#include "resource-manager.h"
#include "material.h"

namespace glrt {
namespace scene {
namespace resources {


template<typename T>
void ResourceManager::addMaterialUser(const Uuid<Material>& material, const Uuid<T>& materialuser)
{
  Index* index = const_cast<Index*>(indexForResourceUuid(material));

  auto& materials = index->materials;
  auto i = materials.find(material);

  if(i != materials.end())
    i.value().addMaterialUser<T>(materialuser);
  else
    qWarning() << "Trying to add a materialUser"<<materialuser<<"to not (yet?) registered material"<<material;
}


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_RESOURCEMANAGER_INL
