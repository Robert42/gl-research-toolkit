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


Node::Component::Component(Node& node, const Uuid<Component>& uuid, bool isMovable)
  : entity(node),
    uuid(uuid),
    isMovable(isMovable)
{
  this->setParent(node.rootComponent());
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

  return parent()->globalCoordFrame() * localCoordFrame();
}


void Node::Component::registerAngelScriptAPIDeclarations()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("NodeComponent", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);

  glrt::Uuid<void>::registerCustomizedUuidType("NodeComponent", false);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

inline Node::Component* createEmptyComponent(Node* node,
                                             const Uuid<Node::Component>& uuid,
                                             bool isMovable)
{
  return new Node::Component(*node, uuid, isMovable);
}

void Node::Component::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  Node::Component::registerAsBaseOfClass<Component>(angelScriptEngine, "NodeComponent");

  r = angelScriptEngine->RegisterObjectMethod("Node", "NodeComponent@ newEmptyComponent(const Uuid<Component> &in uuid, bool isMovable)", AngelScript::asFUNCTION(createEmptyComponent), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace scene
} // namespace glrt

