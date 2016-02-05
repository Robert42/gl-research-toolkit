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
    sceneLayer(sceneLayer),
    uuid(uuid),
    _rootComponent(nullptr)
{
  if(sceneLayer._nodes.contains(uuid))
    throw GLRT_EXCEPTION("Same uuid used twice");

  sceneLayer._nodes.insert(uuid, this);
  sceneLayer.nodeAdded(this);
}

Node::~Node()
{
  QVector<ModularAttribute*> allModularAttributes = this->allModularAttributes();
  for(ModularAttribute* a : allModularAttributes)
    delete a;
  // #TODO instead of deleting it manually, use the QObject system?
  Q_ASSERT(this->allModularAttributes().isEmpty());

  Component* rootComponent = this->rootComponent();
  delete rootComponent;
  Q_ASSERT(this->rootComponent() == nullptr);

  sceneLayer._nodes.remove(uuid);
  sceneLayer.nodeRemoved(this);
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

resources::ResourceManager& Node::resourceManager()
{
  return sceneLayer.scene.resourceManager;
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
  : TickingObject(node.sceneLayer.scene.tickManager, &node),
    node(node),
    uuid(uuid)
{
  node._allModularAttributes.append(this);
}

Node::ModularAttribute::~ModularAttribute()
{
  node._allModularAttributes.removeOne(this);
}


// ======== Node::Component ====================================================


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
  : TickingObject(node.sceneLayer.scene.tickManager, &node),
    node(node),
    parent(parent==nullptr ? node.rootComponent() : parent),
    uuid(uuid),
    _movable(false),
    _visible(true),
    _parentVisible(true),
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

  this->node.sceneLayer.componentAdded(this);

  node.sceneLayer.scene.globalCoordUpdater.addComponent(this);
}

Node::Component::~Component()
{
  node.sceneLayer.componentRemoved(this);

  QVector<Component*> children = this->children();
  for(Component* child : children)
    delete child;
  Q_ASSERT(this->children().isEmpty());

  if(parent)
    parent->_children.removeOne(this);
  else
    node._rootComponent = nullptr;
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


bool Node::Component::movable() const
{
  return _movable || _coordinateIndex==-1;
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

bool Node::Component::visible() const
{
  return _visible && _parentVisible;
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
      node.sceneLayer.componentShown(this);
    else
      node.sceneLayer.componentHidden(this);

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

/*!
Calculates the global transformation of the given component.

\note This Method is able to accept nullptr as this value.
*/
CoordFrame Node::Component::updateGlobalCoordFrame()
{
  if(this == nullptr)
    return CoordFrame();

  if(hasCustomGlobalCoordUpdater())
    _globalCoordFrame = calcGlobalCoordFrameImpl();
  else
    _globalCoordFrame = parent->globalCoordFrame() * localCoordFrame();

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

  return c.scaleFactor!=NAN || c.position!=glm::vec3(NAN) || c.orientation!=glm::quat(NAN, NAN, NAN, NAN);
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
    coordDependencyDepthChanged(this);

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
}

void Node::Component::collectCoordDependencies(CoordDependencySet* dependencySet) const
{
  if(parent != nullptr)
    dependencySet->addDependency(parent);
}




} // namespace scene
} // namespace glrt

