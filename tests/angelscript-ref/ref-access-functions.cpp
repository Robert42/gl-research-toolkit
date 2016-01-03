#include "common.h"


class Graph;
class Node;
class Edge;

class Graph final : public RefCountedObject
{
public:
  static void registerObjectType(AngelScript::asIScriptEngine* engine);
  static void registerObjectMethods(AngelScript::asIScriptEngine* engine);

  const ref<Node>& get_node(int index);
  int add_node(const ref<Node>& node);

private:
  QVector<ref<Edge>> edges;
  QVector<ref<Node>> nodes;
};

class Edge final : public RefCountedObject
{
public:
  static void registerObjectType(AngelScript::asIScriptEngine* engine);
  static void registerObjectMethods(AngelScript::asIScriptEngine* engine);

  const weakref<Graph>& get_graph();
  void set_graph(const weakref<Graph>& graph);

private:
  weakref<Graph> graph;
  ref<Node> from, to;
};

class Node final : public RefCountedObject
{
public:
  static void registerObjectType(AngelScript::asIScriptEngine* engine);
  static void registerObjectMethods(AngelScript::asIScriptEngine* engine);

  const weakref<Graph>& get_graph();
  void set_graph(const weakref<Graph>& graph);

private:
  weakref<Graph> graph;
  QVector<weakref<Edge>> incoming, outgoing;
};

DECLARE_BASECLASS(RefCountedObject, Graph);
DECLARE_BASECLASS(RefCountedObject, Edge);
DECLARE_BASECLASS(RefCountedObject, Node);


// =============================================================================


const ref<Node>& Graph::get_node(int index)
{
  return nodes[index];
}

int Graph::add_node(const ref<Node>& node)
{
  int i = nodes.indexOf(node);
  if(i >= 0)
    return i;

  node->set_graph(this->as_weakref<Graph>());

  nodes.append(node);
  return nodes.length()-1;
}


// -----------------------------------------------------------------------------


const weakref<Graph>& Edge::get_graph()
{
  return graph;
}
void Edge::set_graph(const weakref<Graph>& graph)
{
  this->graph = graph;
}


// -----------------------------------------------------------------------------


const weakref<Graph>& Node::get_graph()
{
  return graph;
}
void Node::set_graph(const weakref<Graph>& graph)
{
  this->graph = graph;
}


// =============================================================================


void Graph::registerObjectType(AngelScript::asIScriptEngine* engine)
{
  const char* name = "Graph";

  int r = 0;
  r = engine->RegisterObjectType(name, 0, AngelScript::asOBJ_REF);AngelScriptCheck(r);

}

void Graph::registerObjectMethods(AngelScript::asIScriptEngine* engine)
{
  const char* name = "Graph";
  typedef Graph T_class;
  typedef base_of<T_class>::type T_parent;

  int r = 0;

  T_parent::registerAsBaseOfClass<T_class>(engine, name);

  r = engine->RegisterObjectMethod(name, "Node@ get_node(int index)", AngelScript::asMETHOD(T_class,get_node), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = engine->RegisterObjectMethod(name, "int add_node(int index, Node@ node)", AngelScript::asMETHOD(T_class,add_node), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
}

void Edge::registerObjectType(AngelScript::asIScriptEngine* engine)
{
  const char* name = "Edge";

  int r = 0;
  r = engine->RegisterObjectType(name, 0, AngelScript::asOBJ_REF);AngelScriptCheck(r);
}

void Edge::registerObjectMethods(AngelScript::asIScriptEngine* engine)
{
  const char* name = "Edge";
  typedef Edge T_class;
  typedef base_of<T_class>::type T_parent;

  int r = 0;

  T_parent::registerAsBaseOfClass<T_class>(engine, name);

  r = engine->RegisterObjectMethod(name, "Graph@ get_graph(int index)", AngelScript::asMETHOD(T_class,get_graph), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = engine->RegisterObjectMethod(name, "void set_graph(Graph@ graph)", AngelScript::asMETHOD(T_class,set_graph), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
}

void Node::registerObjectType(AngelScript::asIScriptEngine* engine)
{
  const char* name = "Node";

  int r = 0;
  r = engine->RegisterObjectType(name, 0, AngelScript::asOBJ_REF);AngelScriptCheck(r);
}

void Node::registerObjectMethods(AngelScript::asIScriptEngine* engine)
{
  const char* name = "Node";
  typedef Node T_class;
  typedef base_of<T_class>::type T_parent;

  int r = 0;

  T_parent::registerAsBaseOfClass<T_class>(engine, name);

  r = engine->RegisterObjectMethod(name, "Graph@ get_graph(int index)", AngelScript::asMETHOD(T_class,get_graph), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = engine->RegisterObjectMethod(name, "void set_graph(Graph@ graph)", AngelScript::asMETHOD(T_class,set_graph), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
}


// =============================================================================


void test_graph_with_accessors(AngelScript::asIScriptEngine* engine)
{
  asDWORD previousMask = engine->SetDefaultAccessMask(ACESS_MASK_GRAPH);

  Graph::registerObjectType(engine);
  Node::registerObjectType(engine);
  Edge::registerObjectType(engine);

  Graph::registerObjectMethods(engine);
  Node::registerObjectMethods(engine);
  Edge::registerObjectMethods(engine);

  engine->SetDefaultAccessMask(previousMask);

  AngelScriptIntegration::ConfigCallScript config;
  config.accessMask = ACESS_MASK_GRAPH;

  AngelScriptIntegration::callScriptExt<void>(engine, SCRIPT_DIR"/graph.as", "void main()", "graph-test", config);
}
