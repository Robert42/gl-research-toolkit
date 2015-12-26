#ifndef GLRT_SCENE_RESOURCES_UUID_H
#define GLRT_SCENE_RESOURCES_UUID_H

#include <glrt/dependencies.h>

namespace glrt {
namespace scene {
namespace resources {

enum class State
{
  NONE,
  REGISTERED,
  LOADING,
  LOADED
};

class ResourceIndex;
class ResourceLoader;

template<class _identifier>
class Uuid
{
public:
  // You have to make sure the given resourceIndex instance lives longer.
  explicit Uuid(const QUuid& uuid,
                ResourceIndex* resourceIndex,
                ResourceLoader* resourceLoader);

  const QUuid& uuid() const;

  State state() const;
  bool isRegistered() const;
  bool isLoading() const;
  bool isLoaded() const;

private:
  QUuid _uuid;
  ResourceIndex* resourceIndex;
  ResourceLoader* resourceLoader;
};

template<class T>
int qHash(const Uuid<T>& uuid, uint seed=0);


class StaticMeshData;
typedef Uuid<StaticMeshData> StaticMeshUuid;


} // namespace resources
} // namespace scene
} // namespace glrt

#include "resource-uuid.inl"

#endif // GLRT_SCENE_RESOURCES_UUID_H
