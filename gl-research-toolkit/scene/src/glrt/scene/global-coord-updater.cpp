#include <glrt/scene/global-coord-updater.h>

namespace glrt {
namespace scene {


GlobalCoordUpdater::GlobalCoordUpdater()
{
}


void GlobalCoordUpdater::updateCoordinates()
{
#if 0
  updateArray();
  fragmented_array.iterate(this);
#endif
}

#if 0
void GlobalCoordUpdater::addComponent(Node::Component* component)
{
  if(component->isStatic())
    staticComponentsToUpdate.append(component);
  else
    fragmented_array.append_copy(component);

  component->updateCoordDependencyDepth();

  connect(component, &GlobalCoordUpdater::destroyed, this, &GlobalCoordUpdater::removeObject);
  connect(component, &Node::Component::coordDependencyDepthChanged, this, &GlobalCoordUpdater::dependencyDepthChanged);
}


void GlobalCoordUpdater::updateArray()
{
  // #ISSUE-61 OMP
  for(int i=0; i<staticComponentsToUpdate.length(); ++i)
  {
    QPointer<Node::Component> componentPtr = staticComponentsToUpdate[i];

    if(componentPtr.isNull())
      continue;

    Node::Component* component = componentPtr.data();

    if(!component->isStatic())
      continue;

    component->updateGlobalCoordFrame();
  }
  staticComponentsToUpdate.clear();


  fragmented_array.updateSegments(this);
}


void GlobalCoordUpdater::removeObject(QObject* object)
{
  fragmented_array.remove(fragmented_array.indexOf_Safe(static_cast<Node::Component*>(object)));
}

void GlobalCoordUpdater::movabilityChanged(Node::Component* component)
{
  if(component->isStatic())
  {
    fragmented_array.remove(fragmented_array.indexOf(component));
    staticComponentsToUpdate.append(component);
  }else
  {
    fragmented_array.append_copy(component);
  }
}

void GlobalCoordUpdater::dependencyDepthChanged(Node::Component* component)
{
  fragmented_array.orderChangedForValue(component);
}
#endif

} // namespace scene
} // namespace glrt
