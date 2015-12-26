#ifndef GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
#define GLRT_SCENE_RESOURCES_RESOURCEINDEX_H

#include "resource-uuid.h"

namespace glrt {
namespace scene {
namespace resources {


class ResourceIndex final
{
public:
  ResourceIndex();

  ResourceIndex(const ResourceIndex&) = delete;
  ResourceIndex(ResourceIndex&&) = delete;
  ResourceIndex& operator=(const ResourceIndex&) = delete;
  ResourceIndex& operator=(ResourceIndex&&) = delete;

  State stateOf(const QUuid& uuid) const;
  bool isRegistered(const QUuid& uuid) const;
  bool isLoading(const QUuid& uuid) const;
  bool isLoaded(const QUuid& uuid) const;

  void startLoading(const QUuid& uuid);
  void loadNow(const QUuid& uuid);

private:
  friend class ResourceLoader;

  QSet<QUuid> unloadedRessources;
  QSet<QUuid> loadingRessources;
  QSet<QUuid> loadedRessources;

  bool classInvariant();
};


} // namespace resources
} // namespace glrt
} // namespace scene

#include "resource-uuid-index.inl"

#endif // GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
