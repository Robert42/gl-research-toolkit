#ifndef GLRT_SCENE_RESOURCES_RESOURCEINDEX_INL
#define GLRT_SCENE_RESOURCES_RESOURCEINDEX_INL

#include "resource-index.h"

namespace glrt {
namespace scene {
namespace resources {

template<class T>
State Uuid<T>::state() const
{
  return resourceIndex->stateOf(this->uuid());
}

template<class T>
bool Uuid<T>::isRegistered() const
{
  return resourceIndex->isRegistered(this->uuid());
}

template<class T>
bool Uuid<T>::isLoading() const
{
  return resourceIndex->isLoading(this->uuid());
}

template<class T>
bool Uuid<T>::isLoaded() const
{
  return resourceIndex->isLoaded(this->uuid());
}


} // namespace resources
} // namespace glrt
} // namespace scene

#endif // GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
