#include "common.h"


class Graph;
class Node;
class Edge;

class Graph final : public RefCountedObject
{
public:
  static void registerObjectType(AngelScript::asIScriptEngine* engine);

  const ref<Node>& get_node(int index)
  {
    return nodes[index];
  }
  int add_node(const ref<Node>& node)
  {
    int i = nodes.indexOf(node);
    if(i >= 0)
      return i;

    nodes.append(node);
    return nodes.length()-1;
  }

private:
  QVector<ref<Edge>> edges;
  QVector<ref<Node>> nodes;
};

class Edge final : public RefCountedObject
{
public:
  static void registerObjectType(AngelScript::asIScriptEngine* engine);

  const weakref<Graph>& get_graph()
  {
    return graph;
  }
  void set_graph(const weakref<Graph>& graph)
  {
    this->graph = graph;
  }

private:
  weakref<Graph> graph;
  ref<Node> from, to;
};

class Node final : public RefCountedObject
{
public:
  static void registerObjectType(AngelScript::asIScriptEngine* engine);

private:
  weakref<Graph> graph;
  QVector<weakref<Edge>> incoming, outgoing;
};

DECLARE_BASECLASS(RefCountedObject, Graph);
DECLARE_BASECLASS(RefCountedObject, Edge);
DECLARE_BASECLASS(RefCountedObject, Node);

void Graph::registerObjectType(AngelScript::asIScriptEngine* engine)
{
  const char* name = "Graph";
  typedef Graph T_class;
  typedef base_of<T_class>::type T_parent;

  int r = 0;
  r = engine->RegisterObjectType(name, 0, AngelScript::asOBJ_REF);AngelScriptCheck(r);

  T_parent::registerAsBaseOfClass<T_class>(engine, name);
}

void Edge::registerObjectType(AngelScript::asIScriptEngine* engine)
{
  const char* name = "Edge";
  typedef Edge T_class;
  typedef base_of<T_class>::type T_parent;

  int r = 0;
  r = engine->RegisterObjectType(name, 0, AngelScript::asOBJ_REF);AngelScriptCheck(r);

  T_parent::registerAsBaseOfClass<T_class>(engine, name);
}

void Node::registerObjectType(AngelScript::asIScriptEngine* engine)
{
  const char* name = "Node";
  typedef Node T_class;
  typedef base_of<T_class>::type T_parent;

  int r = 0;
  r = engine->RegisterObjectType(name, 0, AngelScript::asOBJ_REF);AngelScriptCheck(r);

  T_parent::registerAsBaseOfClass<T_class>(engine, name);
}




void test_graph_with_accessors(AngelScript::asIScriptEngine* engine)
{
  Graph::registerObjectType(engine);
  Node::registerObjectType(engine);
  Edge::registerObjectType(engine);
}
