#ifndef GLRT_DEPENDENCYSET_H
#define GLRT_DEPENDENCYSET_H

#include <QSet>
#include <QQueue>

namespace glrt {


template<typename T>
struct DependencySet final
{
  QSet<const T*> objectsWithCycles;

  DependencySet(const T* originalObject);
  DependencySet(const DependencySet&) = delete;
  DependencySet(DependencySet&&) = delete;
  DependencySet&operator=(const DependencySet&) = delete;
  DependencySet&operator=(DependencySet&&) = delete;

  void addDependency(const T* object);

  bool dependsOn(const T* other) const;
  bool hasCycles() const;
  int depth() const;

private:
  QSet<const T*> visitedDependencies;
  QQueue<const T*> queuedDependencies;
  int _depth;
};


} // namespace glrt

#include "dependency-set.inl"

#endif // GLRT_DEPENDENCYSET_H
