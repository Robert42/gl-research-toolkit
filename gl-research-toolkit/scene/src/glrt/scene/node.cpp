#include <glrt/scene/node.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-layer.h>
#include <glrt/scene/scene-data.h>

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
  : node(node),
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
Node::Component::Component(Node& node, Component* parent, const Uuid<Component>& uuid, DataClass data_class)
  : node(node),
    parent(parent==nullptr ? node.rootComponent() : parent),
    uuid(uuid),
    data_index{makeDynamic(data_class, parent!=nullptr && parent->isDynamic()), 0, 0xffff},
    _visible(true),
    _parentVisible(true),
    _hiddenBecauseDeletedNextFrame(false),
    _coorddependencyDepth(0),
    _coordinateIndex(-1)
{
  if(this->parent !=nullptr)
  {
    Q_ASSERT(&this->node == &this->parent->node);
    this->parent->_children.append(this);
  }else
  {
    Q_ASSERT(node.rootComponent() == nullptr);
    this->node._rootComponent = this;
  }

  Scene::Data::Transformations& transformations = scene().data->transformDataForClass(data_index.data_class);
  Q_ASSERT(transformations.length < transformations.capacity);
  if(Q_UNLIKELY(transformations.length > transformations.capacity))
  {
    qWarning() << "Too many node components";
    std::exit(0);
  }
  transformations.length++;
  const quint16 last_item_index = transformations.last_item_index();

  transformations.component[last_item_index] = this;
  transformations.orientation[last_item_index] = glm::quat::IDENTITY;
  transformations.position[last_item_index] = glm::vec3(0);
  transformations.scaleFactor[last_item_index] = 1;
  transformations.local_coord_frame[last_item_index] = CoordFrame();
  data_index.array_index = last_item_index;
  transformations.numDynamic+=this->isDynamic();
  transformations.dirtyOrder = true;

  scene().componentAdded(this);
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


  Scene::Data::Transformations& transformations = scene().data->transformDataForIndex(data_index);
  Q_ASSERT(transformations.length>0);
  quint16 last_index = transformations.last_item_index();
  quint16 current_index = data_index.array_index;

  transformations.swap_transform_data(current_index, last_index);
  transformations.length--;
  transformations.numDynamic-=this->isDynamic();
  transformations.dirtyOrder = true;
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

inline bool isNull(const void* instance)
{
  return instance == nullptr;
}

/*!
Appends the whole subtree of this component (including this component itself)
to the ggiven vector \a subTree.

\note This Method is able to accept nullptr as this value.
*/
void Node::Component::collectSubtree(QVector<Component*>* subTree)
{
  if(isNull(this))
    return;

  subTree->append(this);

  for(Component* child : children())
    child->collectSubtree(subTree);
}

bool Node::Component::isDynamic() const
{
  return (this->data_index.data_class & DataClass::DYNAMIC) == DataClass::DYNAMIC;
}

bool Node::Component::visible() const
{
  return _visible && _parentVisible && !_hiddenBecauseDeletedNextFrame;
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

Node::Component::DataClass Node::Component::makeDynamic(Node::Component::DataClass dataClass, bool makeDynamic)
{
  if(makeDynamic)
    return DataClass::DYNAMIC | dataClass;
  else
    return dataClass;
}


const CoordFrame& Node::Component::localCoordFrame() const
{
  Scene::Data::Transformations& transformations = scene().data->transformDataForIndex(data_index);
  return transformations.local_coord_frame[data_index.array_index];
}

/*!
Returns the cached global transformation of the given component.

\note The cache ist updated after calling the tick functions of any components.
If your tick function depends on the final global position of a component, use
the slow function calcGlobalCoordFrame to update it first or think, whether
*/
CoordFrame Node::Component::globalCoordFrame() const
{
  Scene::Data::Transformations& transformations = scene().data->transformDataForIndex(data_index);
  return transformations.globalCoordFrame(data_index.array_index);
}

void Node::Component::set_localCoordFrame(const CoordFrame& coordFrame)
{
  scene().data->transformDataForIndex(data_index).local_coord_frame[data_index.array_index] = coordFrame;
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
                                             const Uuid<Node::Component>& uuid,
                                             bool makeDynamic)
{
  return new Node::Component(node, parent, uuid, Node::Component::makeDynamic(Node::Component::DataClass::EMPTY, makeDynamic));
}

void Node::Component::registerAngelScriptAPI()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  Node::Component::registerAsBaseOfClass<Component>(angelScriptEngine, "NodeComponent");

  Node::Component::_registerCreateMethod<decltype(createEmptyComponent), createEmptyComponent>(angelScriptEngine,
                                                                                               "NodeComponent",
                                                                                               "new_EmptyComponent",
                                                                                               "const Uuid<NodeComponent> &in uuid, bool dynamic=false");

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void Node::Component::collectDependencies(CoordDependencySet* dependencySet) const
{
  collectCoordDependencies(dependencySet);

#ifdef QT_DEBUG
  if(dependencySet->originalObject() == this)
  {
    for(const Component* dependency : dependencySet->queuedDependencies() + dependencySet->queuedDependencies())
      if(!this->isDynamic() && dependency->isDynamic())
        qWarning() << "Warning: not dynamic component " << this->uuid << " depending on a dynamic component " << dependency->uuid << ".";
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


} // namespace scene
} // namespace glrt

