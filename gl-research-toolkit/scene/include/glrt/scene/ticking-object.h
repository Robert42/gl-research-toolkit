#ifndef GLRT_SCENE_TICKINGOBJECT_H
#define GLRT_SCENE_TICKINGOBJECT_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/dependency-set.h>
#include <glrt/toolkit/array.h>

namespace glrt {
namespace scene {


class TickManager;
class TickingObject : public QObject
{
  Q_OBJECT
public:
  enum class TickTraits
  {
    // The order explained: This order is used by the Tick manager to decide in
    // which order objects are called. So NoTick must be the last one to prevent
    // gaps. between segments.
    OnlyMainThread,
    Multithreaded,
    NoTick,
  };

  virtual void tick(float timeDelta) const;

  bool tickDependsOn(const TickingObject* other) const;
  int tickDependencyDepth() const;
  int updateTickDependencyDepth();
  virtual TickTraits tickTraits() const;

signals:
  void tickDependencyDepthChanged(TickingObject* sender);

protected:
  typedef DependencySet<TickingObject> TickDependencySet;
  friend struct DependencySet<TickingObject>;

  TickingObject(TickManager& tickManager, QObject* parent = nullptr);
  ~TickingObject();

  void collectDependencies(TickDependencySet* dependencySet) const;

  virtual void collectTickDependencies(TickDependencySet* dependencySet) const;

private:
  int _tickDependencyDepth;
};


} // namespace scene


template<>
struct DefaultTraits<scene::TickingObject::TickTraits>
{
  typedef ArrayTraits_Unordered_Primitive<scene::TickingObject::TickTraits> type;
};

} // namespace glrt

#endif // GLRT_SCENE_TICKINGOBJECT_H
