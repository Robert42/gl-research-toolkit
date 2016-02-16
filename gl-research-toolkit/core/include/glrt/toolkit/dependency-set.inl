#ifndef GLRT_DEPENDENCYSET_INL
#define GLRT_DEPENDENCYSET_INL

#include "dependency-set.h"

namespace glrt {


template<typename T>
DependencySet<T>::DependencySet(const T* originalObject)
  : _originalObject(originalObject)
{
  _depth = -1;

  _queuedDependencies.enqueue(originalObject);

  while(!_queuedDependencies.isEmpty())
  {
    QQueue<const T*> currentDepth;
    currentDepth.swap(_queuedDependencies);

    while(!currentDepth.isEmpty())
    {
      const T* object = currentDepth.dequeue();
      Q_ASSERT(!_visitedDependencies.contains(object));
      _visitedDependencies.insert(object);

      object->collectDependencies(this);
    }

    _depth++;
  }
}

template<typename T>
void DependencySet<T>::addDependency(const T* component)
{
  if(_visitedDependencies.contains(component) || _queuedDependencies.contains(component))
  {
    _objectsWithCycles.insert(component);
  }else
  {
    _queuedDependencies.enqueue(component);
  }
}

template<typename T>
bool DependencySet<T>::dependsOn(const T* other) const
{
  return _visitedDependencies.contains(other);
}

template<typename T>
bool DependencySet<T>::hasCycles() const
{
  return !_objectsWithCycles.isEmpty();
}

template<typename T>
int DependencySet<T>::depth() const
{
  return _depth;
}

template<typename T>
const QSet<const T*>& DependencySet<T>::objectsWithCycles() const
{
  return _objectsWithCycles;
}

template<typename T>
const QSet<const T*>& DependencySet<T>::visitedDependencies() const
{
  return _visitedDependencies;
}

template<typename T>
const QQueue<const T*>& DependencySet<T>::queuedDependencies() const
{
  return _queuedDependencies;
}

template<typename T>
const T* DependencySet<T>::originalObject() const
{
  return _originalObject;
}

} // namespace glrt

#endif // GLRT_DEPENDENCYSET_INL
