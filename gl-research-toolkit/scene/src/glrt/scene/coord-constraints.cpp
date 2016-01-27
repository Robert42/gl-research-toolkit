#include <glrt/scene/coord-constraints.h>

namespace glrt {
namespace scene {

CoordConstraint::CoordConstraint(Node& node, Component* parent, const Uuid<CoordConstraint>& uuid)
  : Component(node, parent, uuid)
{
}

Node::Component* CoordConstraint::target()
{
  return _target;
}

void CoordConstraint::setTarget(Component* target)
{
  if(target != this->_target)
  {
      // Making sure to prevent cycles
    if(target->dependsOn(this))
      throw GLRT_EXCEPTION("CoordConstraints::setTarget: Dependency cycle detected");

    if(this->_target)
      disconnect(this->_target, &Node::Component::dependencyDepthChanged, this, &Node::Component::dependencyDepthChanged);

    _target = target;

    if(this->target())
      connect(this->target(), &Node::Component::dependencyDepthChanged, this, &Node::Component::dependencyDepthChanged);

    targetChanged();
  }
}

void CoordConstraint::collectDependencies(DependencySet* dependencySet) const
{
  if(_target != nullptr)
    dependencySet->addDependency(_target);

  Node::Component::collectDependencies(dependencySet);

}

} // namespace scene
} // namespace glrt
