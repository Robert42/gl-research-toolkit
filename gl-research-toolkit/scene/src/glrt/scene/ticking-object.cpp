#include <glrt/scene/ticking-object.h>
#include <glrt/scene/tick-manager.h>

namespace glrt {
namespace scene {


TickingObject::TickingObject(TickManager& tickManager, QObject* parent)
  : QObject(parent),
    _tickDependencyDepth(0)
{
  tickManager.addTickingObject(this);
}

TickingObject::~TickingObject()
{
}

/*
\warning Never update the Qt eventloop within a tick.  If you do so, assertions might get thrown
*/
void TickingObject::tick(float timeDelta) const
{
  Q_UNUSED(timeDelta);
}

bool TickingObject::tickDependsOn(const TickingObject* other) const
{
  TickDependencySet dependencies(this);

  return dependencies.dependsOn(other);
}

int TickingObject::updateTickDependencyDepth()
{
  TickDependencySet dependencies(this);

  int newDepth = dependencies.depth();

  if(_tickDependencyDepth != newDepth)
    tickDependencyDepthChanged(this);

  return _tickDependencyDepth;
}

int TickingObject::tickDependencyDepth() const
{
  return _tickDependencyDepth;
}

void TickingObject::collectDependencies(TickDependencySet* dependencySet) const
{
  collectTickDependencies(dependencySet);
}

/*!
Defines, whether this object has an impplementation of the tick function which
should be used or not.

\warning this function should always return the same value, no matter when called!

The order of tick is decided using the returned traits and the tick Dependencies.

Each object is guaranteed, that all of its dependency has been executed before
the object.

\list
\li If this function returns TickTraits::NoTick no tick is executed at all.
Thats the safest and most performant solution.
\li If this function returns TickTraits::OnlyMainThread, the tick manager guarantees, that
tick is called in the main thread and the tick manaer also doesn't execute any
other tick function in a nother thread.
\li If this function returns TickTraits::Multithreaded, the tick manager calls the
tick function, in parallel together with other tick function from other tickable
objects with the TickTraits::Multithreaded trait.
\br
\warning
While this can be powerful it is extremely dengerous. You are responisble to make
sure to prevent race condition.
Even if your tick only reads data, you probably want to make sure no one other is
changing the same data in parallel. Don't say you haven't been warned.
\br
Avoid deleting any object directly, instead, call deleteLater()
\br
If you aren't 100% sure that your ticks are race condition free, I recomment to
use the TickTraits::OnlyMainThread trait (which is returned by default).
You can use tick dependencies to ensure, that theese two tick functions aren't called parallel,
but you still have to make sure that no other thread will create any race condition, which isn't trivial.
\endlist

*/
TickingObject::TickTraits TickingObject::tickTraits() const
{
  return TickTraits::NoTick;
}

void TickingObject::collectTickDependencies(TickDependencySet* dependencySet) const
{
  Q_UNUSED(dependencySet);
}


} // namespace scene
} // namespace glrt
