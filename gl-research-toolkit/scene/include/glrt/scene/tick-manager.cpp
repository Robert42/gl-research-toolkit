#include <glrt/scene/tick-manager.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {

TickManager::TickManager()
{
}

TickManager::~TickManager()
{
}

void TickManager::tick(float deltaTime)
{
  for(TickNodeAttribute* a : tickingAttribute)
    a->tick(deltaTime);
}

TickNodeAttribute::TickNodeAttribute(Node& node, const Uuid<glrt::scene::Node::ModularAttribute>& uuid)
  : Node::ModularAttribute(node, uuid)
{
  TickManager& tickManager = node.scene().tickManager;
  tickManager.tickingAttribute.append(this);
}

TickNodeAttribute::~TickNodeAttribute()
{
  TickManager& tickManager = node.scene().tickManager;
  tickManager.tickingAttribute.removeAt(tickManager.tickingAttribute.indexOfFirst(this));
}

} // namespace scene
} // namespace glrt
