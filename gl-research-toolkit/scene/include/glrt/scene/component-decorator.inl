#ifndef GLRT_SCENE_COMPONENTDECORATOR_INL
#define GLRT_SCENE_COMPONENTDECORATOR_INL

#include <glrt/scene/component-decorator.h>
#include <glrt/scene/collect-scene-data.h>

namespace glrt {
namespace scene {


template<class T_Component>
ComponentDecorator<T_Component>::ComponentDecorator(Scene* scene, const DecoratorFunction& decoratorFunction)
  : QObject(scene),
    decoratorFunction(decoratorFunction)
{
  for(T_Component* component : collectAllComponentsWithType<T_Component>(scene))
    addDecoration(component);

  connect(scene, &Scene::componentAdded, this, &ComponentDecorator<T_Component>::handleAddedComponent, Qt::QueuedConnection);
}

template<class T_Component>
void ComponentDecorator<T_Component>::setVisible(bool visible)
{
  this->visible = visible;
  for(Node::Component* decorator : allDecorators)
    decorator->show(visible);
}

template<class T_Component>
void ComponentDecorator<T_Component>::handleAddedComponent(Node::Component* component)
{
  T_Component* c = dynamic_cast<T_Component*>(component);
  if(c)
    addDecoration(c);
}

template<class T_Component>
void ComponentDecorator<T_Component>::addDecoration(T_Component* component)
{
  Node::Component* decoration = decoratorFunction(component);
  if(decoration)
  {
    allDecorators.append(decoration);
    decoration->show(visible);
  }
}


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_COMPONENTDECORATOR_INL
