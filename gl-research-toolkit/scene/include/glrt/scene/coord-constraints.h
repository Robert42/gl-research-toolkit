#ifndef GLRT_SCENE_COORDCONSTRAINTS_H
#define GLRT_SCENE_COORDCONSTRAINTS_H

#include <glrt/scene/node.h>

namespace glrt {
namespace scene {

class CoordConstraint : public Node::Component
{
  Q_OBJECT
  Q_PROPERTY(Component* target READ target WRITE setTarget NOTIFY targetChanged)
public:
  CoordConstraint(Node& node, Component* parent, const Uuid<CoordConstraint>& uuid);

  Component* target();
  void setTarget(Component* component);

signals:
  void targetChanged();

protected:
  void collectCoordDependencies(CoordDependencySet* dependencySet) const override;

private:
  Component* _target;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_COORDCONSTRAINTS_H
