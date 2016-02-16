#ifndef GLRT_DEPENDENCYSET_H
#define GLRT_DEPENDENCYSET_H

#include <QSet>
#include <QQueue>

namespace glrt {


template<typename T>
struct DependencySet final
{
  DependencySet(const T* originalObject);
  DependencySet(const DependencySet&) = delete;
  DependencySet(DependencySet&&) = delete;
  DependencySet&operator=(const DependencySet&) = delete;
  DependencySet&operator=(DependencySet&&) = delete;

  void addDependency(const T* object);

  bool dependsOn(const T* other) const;
  bool hasCycles() const;
  int depth() const;

  const QSet<const T*>& objectsWithCycles() const;
  const QSet<const T*>& visitedDependencies() const;
  const QQueue<const T*>& queuedDependencies() const;
  const T* originalObject() const;

private:
  QSet<const T*> _objectsWithCycles;
  QSet<const T*> _visitedDependencies;
  QQueue<const T*> _queuedDependencies;
  const T* const _originalObject;
  int _depth;
};


} // namespace glrt

#include "dependency-set.inl"

#endif // GLRT_DEPENDENCYSET_H
