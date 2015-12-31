#ifndef GLRT_SCENE_RESOURCES_RESOURCEINDEX_INL
#define GLRT_SCENE_RESOURCES_RESOURCEINDEX_INL

#include "resource-loader.h"

namespace glrt {
namespace scene {
namespace resources {


template<class T>
void Uuid<T>::startLoading() const
{
  resourceLoader->startLoading();
}

template<class T>
bool Uuid<T>::loadNow() const
{
  resourceLoader->loadNow();
}


} // namespace resources
} // namespace glrt
} // namespace scene

#endif // GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
