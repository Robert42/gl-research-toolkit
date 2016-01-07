#ifndef GLRT_UUID_INL
#define GLRT_UUID_INL

#include "uuid.h"

namespace glrt {


template<class T>
Uuid<T>::Uuid(const QUuid& uuid)
  : _uuid(uuid)
{
}


template<class T>
Uuid<T>::operator const QUuid&() const
{
  return this->_uuid;
}


template<class T>
template<class T_target>
Uuid<T_target> Uuid<T>::cast() const
{
  return Uuid<T_target>(this->_uuid);
}


template<class T>
bool Uuid<T>::operator==(const QUuid& other) const
{
  return this->_uuid == other;
}


template<class T>
bool Uuid<T>::operator!=(const QUuid& other) const
{
  return this->_uuid != other;
}


template<class T>
bool Uuid<T>::operator<(const QUuid& other) const
{
  return this->_uuid < other;
}


template<class T>
bool Uuid<T>::operator>(const QUuid& other) const
{
  return this->_uuid > other;
}


template<class T>
bool Uuid<T>::operator<=(const QUuid& other) const
{
  return this->_uuid <= other;
}


template<class T>
bool Uuid<T>::operator>=(const QUuid& other) const
{
  return this->_uuid >= other;
}


template<class T>
int qHash(const Uuid<T>& uuid, uint seed)
{
  return qHash(QUuid(uuid), seed);
}


} // namespace glrt

#endif // GLRT_UUID_H
