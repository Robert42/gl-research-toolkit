#include <glrt/scene/node.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-layer.h>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {


using AngelScriptIntegration::AngelScriptCheck;

inline Node* create_node(SceneLayer* scenelayer, const Uuid<Node>& uuid)
{
  return new Node(*scenelayer, uuid);
}


// ======== Node =============================================================


Node::Node(SceneLayer& sceneLayer, const Uuid<Node>& uuid)
  : QObject(&sceneLayer),
    uuid(uuid),
    _sceneLayer(sceneLayer),
    _rootComponent(nullptr)
{
  if(sceneLayer._nodes.contains(uuid))
    throw GLRT_EXCEPTION("Same uuid used twice");

  sceneLayer._nodes.insert(uuid, this);
  scene().nodeAdded(this);
}

Node::~Node()
{
  QVector<ModularAttribute*> allModularAttributes = this->allModularAttributes();
  for(ModularAttribute* a : allModularAttributes)
    delete a;

  Q_ASSERT(this->allModularAttributes().isEmpty());

  Component* rootComponent = this->rootComponent();
  delete rootComponent;
  Q_ASSERT(this->rootComponent() == nullptr);

  sceneLayer()._nodes.remove(uuid);
  scene().nodeRemoved(this);
}

Scene& Node::scene()
{
  return _sceneLayer.scene();
}

const Scene& Node::scene()const
{
  return _sceneLayer.scene();
}

SceneLayer& Node::sceneLayer()
{
  return _sceneLayer;
}

const SceneLayer& Node::sceneLayer()const
{
  return _sceneLayer;
}

resources::ResourceManager& Node::resourceManager()
{
  return _sceneLayer.scene().resourceManager;
}

const resources::ResourceManager& Node::resourceManager() const
{
  return _sceneLayer.scene().resourceManager;
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

void Node::registerAngelScriptAPIDeclarations()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("Node", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);

  glrt::Uuid<void>::registerCustomizedUuidType("Node", false);

  Component::registerAngelScriptAPIDeclarations();

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void Node::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectMethod("SceneLayer", "Node@ newNode(Uuid<Node> &in uuid)", AngelScript::asFUNCTION(create_node), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("Node", "NodeComponent@ get_rootComponent()", AngelScript::asMETHOD(Node, rootComponent), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  Component::registerAngelScriptAPI();

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


// ======== Node::ModularAttribute =============================================


Node::ModularAttribute::ModularAttribute(Node& node, const Uuid<ModularAttribute>& uuid)
  : TickingObject(node.scene().tickManager, &node),
    node(node),
    uuid(uuid)
{
  node._allModularAttributes.append(this);
}

Node::ModularAttribute::~ModularAttribute()
{
  node._allModularAttributes.removeOne(this);
}


Scene& Node::ModularAttribute::scene()
{
  return node.scene();
}

const Scene& Node::ModularAttribute::scene()const
{
  return node.scene();
}

SceneLayer& Node::ModularAttribute::sceneLayer()
{
  return node.sceneLayer();
}

const SceneLayer& Node::ModularAttribute::sceneLayer()const
{
  return node.sceneLayer();
}

resources::ResourceManager& Node::ModularAttribute::resourceManager()
{
  return node.resourceManager();
}

const resources::ResourceManager& Node::ModularAttribute::resourceManager() const
{
  return node.resourceManager();
}


// ======== Node::Component ====================================================

/*!
\property Node::Component::mayBecomeMovable

This property is just a hint. For example the renderer might decide to order not
movable components, which may be come movable later on between movable and not
movable componetns in an array. this way, after becoming movable, a smaller part
of components have to be resorted.
*/

/*!
Constructs a new Components and adds it to the given \a node.

Optionally, you can pass a \a parent component. Note, that you are allowed to
pass nullptr as \a parent.
\br
In this case, the component will be added as rootComponent, if there is'nt already
one. If there's aredy a rootComponent, the rootComponent will be the parent of
the new component.

\note Once established, a parent/child component relationship can be
only changed by deleting the child or parent component.

This component will have the given \a uuid.
*/
Node::Component::Component(Node& node, Component* parent, const Uuid<Component>& uuid)
  : TickingObject(node.scene().tickManager, &node),
    node(node),
    parent(parent==nullptr ? node.rootComponent() : parent),
    uuid(uuid),
    _globalCoordFrame(glm::uninitialize),
    _movable(false),
    _mayBecomeMovable(false),
    _visible(true),
    _parentVisible(true),
    _hiddenBecauseDeletedNextFrame(false),
    _hasAABB(false),
    _coorddependencyDepth(0),
    _coordinateIndex(-1)
{
  if(this->parent !=nullptr)
  {
    Q_ASSERT(&this->node == &this->parent->node);
    this->parent->_children.append(this);

    connect(this->parent, &Node::Component::coordDependencyDepthChanged, this, &Node::Component::coordDependencyDepthChanged);
  }else
  {
    Q_ASSERT(node.rootComponent() == nullptr);
    this->node._rootComponent = this;
  }

  scene().componentAdded(this);

  scene().globalCoordUpdater.addComponent(this);
}

/*!
\note While calling the destructor, hideInDestructor is called, you
are guaranteed that the component is beeing hidden.
*/
Node::Component::~Component()
{
  hideInDestructor();

  scene().componentRemoved(this);

  QVector<Component*> children = this->children();
  for(Component* child : children)
    delete child;
  Q_ASSERT(this->children().isEmpty());

  if(parent)
    parent->_children.removeOne(this);
  else
    node._rootComponent = nullptr;
}

Scene& Node::Component::scene()
{
  return node.scene();
}

const Scene& Node::Component::scene()const
{
  return node.scene();
}

SceneLayer& Node::Component::sceneLayer()
{
  return node.sceneLayer();
}

const SceneLayer& Node::Component::sceneLayer()const
{
  return node.sceneLayer();
}

resources::ResourceManager& Node::Component::resourceManager()
{
  return node.resourceManager();
}

const resources::ResourceManager& Node::Component::resourceManager() const
{
  return node.resourceManager();
}


const QVector<glrt::scene::Node::Component*>& Node::Component::children() const
{
  return this->_children;
}

/*!
Appends the whole subtree of this component (including this component itself)
to the ggiven vector \a subTree.

\note This Method is able to accept nullptr as this value.
*/
void Node::Component::collectSubtree(QVector<Component*>* subTree)
{
  if(this == nullptr)
    return;

  subTree->append(this);

  for(Component* child : children())
    child->collectSubtree(subTree);
}

Node::Component::MovabilityHint Node::Component::movabilityHint() const
{
  int movable = static_cast<int>(this->movable());
  int mayBecomeMovable = static_cast<int>(this->mayBecomeMovable());
  Q_ASSERT(movable<=1);
  Q_ASSERT(mayBecomeMovable<=1);
  int hintValue = (movable<<1) | (mayBecomeMovable & ~movable);

  Q_ASSERT(hintValue == static_cast<int>(MovabilityHint::STATIC) ||
           hintValue == static_cast<int>(MovabilityHint::MAY_BECOME_MOVABLE) ||
           hintValue == static_cast<int>(MovabilityHint::MOVABLE));

  return static_cast<MovabilityHint>(hintValue);
}

bool Node::Component::movable() const
{
  return _movable || _coordinateIndex==-1;
}

bool Node::Component::mayBecomeMovable() const
{
  return _mayBecomeMovable;
}

void Node::Component::setMovable(bool movable)
{
  if(this->movable() != movable)
  {
    this->_movable = movable;
    movableChanged(movable);
    componentMovabilityChanged(this);
  }
}

void Node::Component::setMayBecomeMovable(bool mayBecomeMovable)
{
  if(this->mayBecomeMovable() != mayBecomeMovable)
    this->_mayBecomeMovable = mayBecomeMovable;
}

bool Node::Component::visible() const
{
  return _visible && _parentVisible && !_hiddenBecauseDeletedNextFrame;
}


bool Node::Component::hasAABB() const
{
  return _hasAABB;
}

void Node::Component::setVisible(bool visible)
{
  bool prevVisibility = this->visible();
  this->_visible = visible;
  updateVisibility(prevVisibility);
}

void Node::Component::updateVisibility(bool prevVisibility)
{
  bool currentVisibility = this->visible();
  if(currentVisibility != prevVisibility)
  {
    visibleChanged(currentVisibility);
    componentVisibilityChanged(this);
    if(currentVisibility)
      scene().componentShown(this);
    else
      scene().componentHidden(this);

    for(Component* child : _children)
    {
      bool prevVisibility = child->visible();
      child->_parentVisible = currentVisibility;
      child->updateVisibility(prevVisibility);
    }
  }
}

void Node::Component::show(bool show)
{
  setVisible(show);
}

void Node::Component::hide(bool hide)
{
  setVisible(!hide);
}

void Node::Component::hideNowAndDeleteLater()
{
  hideInDestructor();
  this->deleteLater();
}

void Node::Component::hideInDestructor()
{
  bool prevVisibility = this->visible();
  _hiddenBecauseDeletedNextFrame = true;
  updateVisibility(prevVisibility);
}


CoordFrame Node::Component::localCoordFrame() const
{
  return _localCoordFrame;
}

/*!
Returns the cached global transformation of the given component.

\note The cache ist updated after calling the tick functions of any components.
If your tick function depends on the final global position of a component, use
the slow function calcGlobalCoordFrame to update it first or think, whether
*/
CoordFrame Node::Component::globalCoordFrame() const
{
  return _globalCoordFrame;
}

quint64 Node::Component::zOrder() const
{
  // #TODO: return a real zCode
  return quint64(this);
}

/*!
Calculates the global transformation of the given component.

\note This Method is able to accept nullptr as this value.
*/
CoordFrame Node::Component::updateGlobalCoordFrame()
{
  if(this == nullptr)
    return CoordFrame();

  if(Q_UNLIKELY(hasCustomGlobalCoordUpdater()))
    _globalCoordFrame = calcGlobalCoordFrameImpl();
  else if(Q_LIKELY(parent != nullptr))
    _globalCoordFrame = parent->globalCoordFrame() * localCoordFrame();
  else
    _globalCoordFrame = localCoordFrame();

  if(Q_LIKELY(hasAABB()))
    reinterpret_cast<ComponentWithAABB*>(this)->expandSceneAABB();

  return _globalCoordFrame;
}


/*!
This function allows to use a customzed calculation for the global coord frame
of a compent.

The default implementation returns a coord frame containing only nans to signalize
not to use this virtual function to improve performance.

\warning Either always return a coord frame consisting only of NANs or never.

\warning This functions will be called multithreaded, so don't depend on anything
different than this component itself or it's CoordDependencies.
\br
Don't change any state of any object, just calculate the new GlobalCoordinate and
return it.
\br
Don't change anything, expecially don't change the movability or delete any objects.
*/
CoordFrame Node::Component::calcGlobalCoordFrameImpl() const
{
  return CoordFrame(glm::vec3(NAN), glm::quat(NAN, NAN, NAN, NAN), NAN);
}

bool Node::Component::hasCustomGlobalCoordUpdater() const
{
  CoordFrame c = calcGlobalCoordFrameImpl();

  return !std::isnan(c.scaleFactor);
}

void Node::Component::set_localCoordFrame(const CoordFrame& coordFrame)
{
  if(!movable())
  {
    qWarning() << "Trying to move not movable component";
    return;
  }

  this->_localCoordFrame = coordFrame;
}


bool Node::Component::coordDependsOn(const Component* other) const
{
  CoordDependencySet dependencies(this);

  return dependencies.dependsOn(other);
}

int Node::Component::updateCoordDependencyDepth()
{
  CoordDependencySet dependencies(this);

  int newDepth = dependencies.depth();

  if(_coorddependencyDepth != newDepth)
  {
    _coorddependencyDepth = newDepth;
    coordDependencyDepthChanged(this);
  }

  return _coorddependencyDepth;
}

int Node::Component::coordDependencyDepth() const
{
  return _coorddependencyDepth;
}


void Node::Component::registerAngelScriptAPIDeclarations()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("NodeComponent", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);

  glrt::Uuid<void>::registerCustomizedUuidType("NodeComponent", false);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

inline Node::Component* createEmptyComponent(Node& node,
                                             Node::Component* parent,
                                             const Uuid<Node::Component>& uuid)
{
  return new Node::Component(node, parent, uuid);
}

void Node::Component::registerAngelScriptAPI()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  Node::Component::registerAsBaseOfClass<Component>(angelScriptEngine, "NodeComponent");

  Node::Component::_registerCreateMethod<decltype(createEmptyComponent), createEmptyComponent>(angelScriptEngine,
                                                                                               "NodeComponent",
                                                                                               "new_EmptyComponent",
                                                                                               "const Uuid<NodeComponent> &in uuid");

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void Node::Component::collectDependencies(TickDependencySet* dependencySet) const
{
  TickingObject::collectDependencies(dependencySet);
}

void Node::Component::collectDependencies(CoordDependencySet* dependencySet) const
{
  collectCoordDependencies(dependencySet);

#ifdef QT_DEBUG
  if(dependencySet->originalObject() == this)
  {
    for(const Component* dependency : dependencySet->queuedDependencies() + dependencySet->queuedDependencies())
      if(!this->movable() && dependency->movable())
        qWarning() << "Warning: not movable object " << this->uuid << " depending on a movable component " << dependency->uuid << ".";
  }

  if(!dependencySet->objectsWithCycles().isEmpty())
    qWarning() << "Warning: dependency cycles detected (the component " << this->uuid<<" has detected a dependency cycle)";
#endif
}

void Node::Component::collectCoordDependencies(CoordDependencySet* dependencySet) const
{
  if(parent != nullptr)
    dependencySet->addDependency(parent);
}


// ======== ComponentWithAABB ====================================================


ComponentWithAABB::ComponentWithAABB(Node& node, Component* parent, const Uuid<Component>& uuid)
  : Node::Component(node, parent, uuid),
    localAabb(AABB::invalid())
{
  _hasAABB = true;
}

ComponentWithAABB::~ComponentWithAABB()
{
}

AABB ComponentWithAABB::globalAABB() const
{
  return localAabb.aabbOfTransformedBoundingBox(this->globalCoordFrame());
}

void ComponentWithAABB::expandSceneAABB()
{
  scene().aabb |= this->localAabb;
}


} // namespace scene
} // namespace glrt

