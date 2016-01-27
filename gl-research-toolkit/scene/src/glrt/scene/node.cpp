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
  : sceneLayer(sceneLayer),
    uuid(uuid),
    _rootComponent(nullptr)
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
  // #TODO instead of deleting it manually, use the QObject system?
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

// ======== Node::ModularAttribute ===========================================


Node::ModularAttribute::ModularAttribute(Node& node, const Uuid<ModularAttribute>& uuid)
  : node(node),
    uuid(uuid)
{
  node._allModularAttributes.append(this);
}

Node::ModularAttribute::~ModularAttribute()
{
  node._allModularAttributes.removeOne(this);
}


// ======== Node::Component ==================================================


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
  : node(node),
    parent(parent==nullptr ? node.rootComponent() : parent),
    uuid(uuid),
    isMovable(false),
    _dependencyDepth(0)
{
  if(this->parent !=nullptr)
  {
    Q_ASSERT(&this->node == &this->parent->node);
    this->parent->_children.append(this);

    connect(this->parent, &Node::Component::dependencyDepthChanged, this, &Node::Component::dependencyDepthChanged);
  }else
  {
    Q_ASSERT(node.rootComponent() == nullptr);
    node._rootComponent = this;
  }
}

Node::Component::~Component()
{
  if(parent)
    parent->_children.removeOne(this);

  QVector<Component*> children = this->children();
  for(Component* child : children)
    delete child;
  Q_ASSERT(children.isEmpty());
}


const QVector<glrt::scene::Node::Component*>& Node::Component::children() const
{
  return this->_children;
}

/*!
Appends the wwhole subtree of this component (including this component itself)
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


CoordFrame Node::Component::localCoordFrame() const
{
  return _localCoordFrame;
}


void Node::Component::set_localCoordFrame(const CoordFrame& coordFrame)
{
  this->_localCoordFrame = coordFrame;
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

  return parent->globalCoordFrame() * localCoordFrame();
}

bool Node::Component::dependsOn(const Component* other) const
{
  DependencySet dependencies(this);

  return dependencies.dependsOn(other);
}

int Node::Component::updateDependencyDepth()
{
  DependencySet dependencies(this);

  _dependencyDepth = dependencies.depth();

  return _dependencyDepth;
}

void Node::Component::collectDependencies(DependencySet* dependencySet) const
{
  if(parent != nullptr)
    dependencySet->addDependency(parent);
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


// ======== Node::Component::DependencySet =====================================

Node::Component::DependencySet::DependencySet(const Component* component)
{
  _depth = -1;

  queuedDependencies.enqueue(component);

  while(!queuedDependencies.isEmpty())
  {
    QQueue<const Component*> currentDepth;

    currentDepth.swap(queuedDependencies);

    while(!currentDepth.isEmpty())
    {
      const Component* component = currentDepth.dequeue();
      Q_ASSERT(!visitedDependencies.contains(component));
      visitedDependencies.insert(component);

      component->collectDependencies(this);
    }

    _depth++;
  }
}

void Node::Component::DependencySet::addDependency(const Component* component)
{
  if(visitedDependencies.contains(component) || queuedDependencies.contains(component))
  {
    componentsWithCycles.insert(component);
  }else
  {
    queuedDependencies.enqueue(component);
  }
}

bool Node::Component::DependencySet::dependsOn(const Component* other) const
{
  return visitedDependencies.contains(other);
}

bool Node::Component::DependencySet::hasCycles() const
{
  return !componentsWithCycles.isEmpty();
}

int Node::Component::DependencySet::depth() const
{
  return _depth;
}


} // namespace scene
} // namespace glrt

