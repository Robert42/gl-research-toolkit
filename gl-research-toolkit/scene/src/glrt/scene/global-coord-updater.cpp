#include <glrt/scene/global-coord-updater.h>

namespace glrt {
namespace scene {


GlobalCoordUpdater::GlobalCoordUpdater()
{
}


void GlobalCoordUpdater::updateCoordinages()
{
  updateArray();
// TODO: fragmented_array.iterate();
}


void GlobalCoordUpdater::addComponent(Node::Component* component)
{
  if(component->movable())
    fragmented_array.append_copy(component);
}


void GlobalCoordUpdater::updateArray()
{
  // TODO:
}


void GlobalCoordUpdater::removeObject(QObject* object)
{
  fragmented_array.remove(static_cast<Node::Component*>(object));
}

void GlobalCoordUpdater::movabilityChanged(Node::Component* component)
{
  if(!component->movable())
  {
    fragmented_array.remove(component);
    staticComponentsToUpdate.insert(component);
  }else
  {
  }
}


} // namespace scene
} // namespace glrt
