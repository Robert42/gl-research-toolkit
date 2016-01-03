#include <glrt/scene/entity.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


// ======== Entity =============================================================


Entity::Entity(Scene& scene, const Uuid<Entity>& uuid)
  : scene(scene),
    uuid(uuid)
{
}

Entity::~Entity()
{
  QVector<ModularAttribute*> allModularAttributes = this->allModularAttributes();
  for(ModularAttribute* a : allModularAttributes)
    delete a;
  Q_ASSERT(this->allModularAttributes().isEmpty());

  Component* rootComponent = this->rootComponent();
  delete rootComponent;
  Q_ASSERT(rootComponent == nullptr);
}

CoordFrame Entity::globalCoordFrame() const
{
  return rootComponent()->globalCoordFrame();
}

const QVector<Entity::ModularAttribute*>& Entity::allModularAttributes()
{
  return _allModularAttributes;
}

QVector<Entity::Component*> Entity::allComponents() const
{
  QVector<Entity::Component*> components;
  rootComponent()->collectSubtree(&components);
  return components;
}

Entity::Component* Entity::rootComponent() const
{
  return _rootComponent;
}

// ======== Entity::ModularAttribute ===========================================


Entity::ModularAttribute::ModularAttribute(Entity& entity, const Uuid<ModularAttribute>& uuid)
  : entity(entity),
    uuid(uuid)
{
  entity._allModularAttributes.append(this);
}

Entity::ModularAttribute::~ModularAttribute()
{
  entity._allModularAttributes.removeOne(this);
}


// ======== Entity::Component ==================================================


Entity::Component::Component(Entity& entity, const Uuid<Component>& uuid, bool isMovable)
  : entity(entity),
    uuid(uuid),
    isMovable(isMovable)
{
  this->setParent(entity.rootComponent());
}

Entity::Component::~Component()
{
  if(parent())
    parent()->_children.removeOne(this);
  this->_parent = nullptr;

  QVector<Component*> children = this->children();
  for(Component* child : children)
    delete child;
  Q_ASSERT(children.isEmpty());
}


Entity::Component* Entity::Component::parent() const
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
void Entity::Component::setParent(Component* component)
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


QVector<Entity::Component*> Entity::Component::children() const
{
  if(this == nullptr)
    return QVector<Entity::Component*>();

  return this->_children;
}

/*!
 * Appends the wwhole subtree of this component (including this component itself)
 * to the ggiven vector \a subTree.
 *
 * \note This Method is able to accept nullptr as this value.
 */
void Entity::Component::collectSubtree(QVector<Component*>* subTree)
{
  if(this == nullptr)
    return;

  subTree->append(this);

  for(Component* child : children())
    child->collectSubtree(subTree);
}


CoordFrame Entity::Component::localCoordFrame() const
{
  return _localCoordFrame;
}

/*!
 * Returns the global transformation of the given component.
 *
 * \note This Method is able to accept nullptr as this value.
 */
CoordFrame Entity::Component::globalCoordFrame() const
{
  if(this == nullptr)
    return CoordFrame();

  return parent()->globalCoordFrame() * localCoordFrame();
}



} // namespace scene
} // namespace glrt

