#ifndef GLRT_UUID_INL
#define GLRT_UUID_INL

#include "uuid.h"

namespace glrt {


template<class T>
Uuid<T>::Uuid(const QUuid& uuid)
  : _index(uuid)
{
}

template<class T>
Uuid<T>::Uuid(UuidIndex index)
  : _index(index)
{
}


template<class T>
Uuid<T>::operator QUuid() const
{
  return toQUuid();
}


template<class T>
QUuid Uuid<T>::toQUuid() const
{
  return this->_index.toQUuid();
}


template<class T>
template<class T_target>
Uuid<T_target> Uuid<T>::cast() const
{
  return Uuid<T_target>(this->_index);
}


template<class T>
bool Uuid<T>::operator==(const this_type& other) const
{
  return this->_index.index == other._index.index;
}


template<class T>
bool Uuid<T>::operator!=(const this_type& other) const
{
  return this->_index.index != other._index.index;
}


template<class T>
bool Uuid<T>::operator<(const this_type& other) const
{
  return this->_index.index < other._index.index;
}


template<class T>
bool Uuid<T>::operator>(const this_type& other) const
{
  return this->_index.index > other._index.index;
}


template<class T>
bool Uuid<T>::operator<=(const this_type& other) const
{
  return this->_index.index <= other._index.index;
}


template<class T>
bool Uuid<T>::operator>=(const this_type& other) const
{
  return this->_index.index >= other._index.index;
}


template<class T>
UuidIndex Uuid<T>::index() const
{
  return _index;
}


inline int qHash(UuidIndex uuid)
{
  return ::qHash(uuid.index);
}

template<class T>
int qHash(const Uuid<T>& uuid)
{
  return qHash(uuid.index());
}


} // namespace glrt

#endif // GLRT_UUID_H
