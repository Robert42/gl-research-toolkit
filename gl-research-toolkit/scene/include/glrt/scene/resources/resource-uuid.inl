#ifndef GLRT_SCENE_RESOURCES_RESOURCEINDEX_INL
#define GLRT_SCENE_RESOURCES_RESOURCEINDEX_INL

#include "resource-uuid.h"

namespace glrt {
namespace scene {
namespace resources {

template<class T>
Uuid<T>::Uuid(const QUuid& uuid)
  : _uuid(uuid)
{
}

template<class T>
const QUuid& Uuid<T>::uuid() const
{
  return this->_uuid;
}

template<class T>
int qHash(const Uuid<T>& uuid, uint seed)
{
  return qHash(uuid._uuid, seed);
}


} // namespace resources
} // namespace glrt
} // namespace scene

#endif // GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
