#ifndef GLRT_DEPENDENCYSET_INL
#define GLRT_DEPENDENCYSET_INL

#include "dependency-set.h"

namespace glrt {


template<typename T>
DependencySet<T>::DependencySet(const T* originalObject)
{
  _depth = -1;

  queuedDependencies.enqueue(originalObject);

  while(!queuedDependencies.isEmpty())
  {
    QQueue<const T*> currentDepth(std::move(queuedDependencies));

    while(!currentDepth.isEmpty())
    {
      const T* object = currentDepth.dequeue();
      Q_ASSERT(!visitedDependencies.contains(object));
      visitedDependencies.insert(object);

      object->collectDependencies(this);
    }

    _depth++;
  }
}

template<typename T>
void DependencySet<T>::addDependency(const T* component)
{
  if(visitedDependencies.contains(component) || queuedDependencies.contains(component))
  {
    objectsWithCycles.insert(component);
  }else
  {
    queuedDependencies.enqueue(component);
  }
}

template<typename T>
bool DependencySet<T>::dependsOn(const T* other) const
{
  return visitedDependencies.contains(other);
}

template<typename T>
bool DependencySet<T>::hasCycles() const
{
  return !objectsWithCycles.isEmpty();
}

template<typename T>
int DependencySet<T>::depth() const
{
  return _depth;
}


} // namespace glrt

#endif // GLRT_DEPENDENCYSET_INL
