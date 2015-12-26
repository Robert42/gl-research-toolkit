#ifndef GLRT_SCENE_RESOURCE_RESOURCELOADER_H
#define GLRT_SCENE_RESOURCE_RESOURCELOADER_H

#include "resource-uuid.h"

namespace glrt {
namespace scene {
namespace resource {

class ResourceLoader
{
public:
  ResourceLoader();

  void startLoading(const QUuid& uuid);
  void loadNow(const QUuid& uuid);
};

} // namespace resource
} // namespace scene
} // namespace glrt

#include "resource-uuid-loader.inl"

#endif // GLRT_SCENE_RESOURCE_RESOURCELOADER_H
