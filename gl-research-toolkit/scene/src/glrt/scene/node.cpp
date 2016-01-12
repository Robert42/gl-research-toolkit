#include <glrt/scene/node.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-layer.h>

namespace glrt {
namespace scene {


// ======== Node =============================================================


Node::Node(SceneLayer& sceneLayer, const Uuid<Node>& uuid)
  : sceneLayer(sceneLayer),
    uuid(uuid)
{
  if(sceneLayer._nodes.contains(uuid))
    throw GLRT_EXCEPTION("Same uuid used twice");

  sceneLayer._nodes[uuid] = this;
}

Node::~Node()
{
  sceneLayer._nodes.remove(uuid);

  QVector<ModularAttribute*> allModularAttributes = this->allModularAttributes();
  for(ModularAttribute* a : allModularAttributes)
    delete a;
  Q_ASSERT(this->allModularAttributes().isEmpty());

  Component* rootComponent = this->rootComponent();
  delete rootComponent;
  Q_ASSERT(rootComponent == nullptr);
}

CoordFrame Node::globalCoordFrame() const
{
  return rootComponent()->globalCoordFrame();
}

const QVector<Node::ModularAttribute*>& Node::allModularAttributes()
{
  return _allModularAttributes;
}

QVector<Node::Component*> Node::allComponents() const
{
  QVector<Node::Component*> components;
  rootComponent()->collectSubtree(&components);
  return components;
}

Node::Component* Node::rootComponent() const
{
  return _rootComponent;
}

// ======== Node::ModularAttribute ===========================================


Node::ModularAttribute::ModularAttribute(Node& entity, const Uuid<ModularAttribute>& uuid)
  : entity(entity),
    uuid(uuid)
{
  entity._allModularAttributes.append(this);
}

Node::ModularAttribute::~ModularAttribute()
{
  entity._allModularAttributes.removeOne(this);
}


// ======== Node::Component ==================================================


Node::Component::Component(Node& entity, const Uuid<Component>& uuid, bool isMovable)
  : entity(entity),
    uuid(uuid),
    isMovable(isMovable)
{
  this->setParent(entity.rootComponent());
}

Node::Component::~Component()
{
  if(parent())
    parent()->_children.removeOne(this);
  this->_parent = nullptr;

  QVector<Component*> children = this->children();
  for(Component* child : children)
    delete child;
  Q_ASSERT(children.isEmpty());
}


Node::Component* Node::Component::parent() const
{
  if(this == nullptr)
    return nullptr;
  return this->_parent;
}

/*!
 * Set the \a component to be the parent of this component.
 *
 * \note This Method is able to accept nullptr as this value and also component to be nullptr.
 */
void Node::Component::setParent(Component* component)
{
  if(this == nullptr)
    return;

  if(component == nullptr)
  {
    if(this->entity.rootComponent() != nullptr)
      throw GLRT_EXCEPTION("Only one root allowed at once");

    if(this->parent() != nullptr)
      this->parent()->_children.removeOne(this);

    this->_parent = nullptr;
    this->entity._rootComponent = this;
  }else
  {
    Q_ASSERT(&component->entity == &this->entity);

    if(this->parent() != nullptr)
      this->parent()->_children.removeOne(this);

    this->_parent = component;
    component->_children.append(this);
  }
}


QVector<Node::Component*> Node::Component::children() const
{
  if(this == nullptr)
    return QVector<Node::Component*>();

  return this->_children;
}

/*!
 * Appends the wwhole subtree of this component (including this component itself)
 * to the ggiven vector \a subTree.
 *
 * \note This Method is able to accept nullptr as this value.
 */
void Node::Component::collectSubtree(QVector<Component*>* subTree)
{
  if(this == nullptr)
    return;

  subTree->append(this);

  for(Component* child : children())
    child->collectSubtree(subTree);
}


CoordFrame Node::Component::localCoordFrame() const
{
  return _localCoordFrame;
}

/*!
 * Returns the global transformation of the given component.
 *
 * \note This Method is able to accept nullptr as this value.
 */
CoordFrame Node::Component::globalCoordFrame() const
{
  if(this == nullptr)
    return CoordFrame();

  return parent()->globalCoordFrame() * localCoordFrame();
}



} // namespace scene
} // namespace glrt

