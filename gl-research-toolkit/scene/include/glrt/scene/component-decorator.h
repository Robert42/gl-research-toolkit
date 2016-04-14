#ifndef GLRT_SCENE_COMPONENTDECORATOR_H
#define GLRT_SCENE_COMPONENTDECORATOR_H

#include <glrt/scene/scene.h>
#include <glrt/scene/node.h>

namespace glrt {
namespace scene {

template<class T_Component>
class ComponentDecorator : public QObject
{
  static_assert(std::is_base_of<glrt::scene::Node::Component, T_Component>::value==true, "T_Component must ineherit from glrt::scene::Node::Component");
public:
  typedef std::function<Node::Component*(T_Component*)> DecoratorFunction;

  explicit ComponentDecorator(Scene* scene, const DecoratorFunction& decoratorFunction);

  void setVisible(bool visible);

private:
  DecoratorFunction decoratorFunction;
  QVector<Node::Component*> allDecorators;
  bool visible = false;

  void addDecoration(T_Component* c);

  void handleAddedComponent(Node::Component* component);
  void handleRemovedComponent(Node::Component* component);
};

} // namespace scene
} // namespace glrt

#include "component-decorator.inl"

#endif // GLRT_SCENE_COMPONENTDECORATOR_H
