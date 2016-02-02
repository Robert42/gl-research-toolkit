#ifndef GLRT_SCENE_TICKINGOBJECT_H
#define GLRT_SCENE_TICKINGOBJECT_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/dependency-set.h>

namespace glrt {
namespace scene {


class TickManager;
class TickingObject : public QObject
{
  Q_OBJECT
public:
  enum class TickTraits
  {
    NoTick,
    OnlyMainThread,
    Multithreaded,
  };

  virtual void tick(float timeDelta) const;

  bool tickDependsOn(const TickingObject* other) const;
  int updateTickDependencyDepth();
  virtual TickTraits tickTraits() const;

signals:
  void tickDependencyDepthChanged(TickingObject* sender);

protected:
  typedef DependencySet<TickingObject> TickDependencySet;
  friend struct DependencySet<TickingObject>;

  TickingObject(TickManager& tickManager);
  ~TickingObject();

  void collectDependencies(TickDependencySet* dependencySet) const;

  virtual void collectTickDependencies(TickDependencySet* dependencySet) const;

private:
  TickManager& tickManager;
  int _tickDependencyDepth;
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_TICKINGOBJECT_H
