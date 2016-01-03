#include <glrt/dependencies.h>
#include <QApplication>

#include <sdk/add_on/scriptstdstring/scriptstdstring.h>
#include <sdk/add_on/scriptarray/scriptarray.h>

using AngelScript::asDWORD;
using AngelScriptIntegration::AngelScriptCheck;
using AngelScriptIntegration::RefCountedObject;
using AngelScriptIntegration::ref;
using AngelScriptIntegration::weakref;

class Graph;
class Node;
class Edge;

class Graph final : public RefCountedObject
{
public:
  static void registerObjectType(AngelScript::asIScriptEngine* engine);

private:
  QVector<ref<Edge>> edges;
  QVector<ref<Node>> nodes;
};

class Edge final : public RefCountedObject
{
public:
  static void registerObjectType(AngelScript::asIScriptEngine* engine);

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
  int r = 0;

  r = engine->RegisterObjectType("Graph", 0, AngelScript::asOBJ_REF);AngelScriptCheck(r);

  base_of<Graph>::type::registerAsBaseOfClass<Graph>(engine, "Graph");
}

void Edge::registerObjectType(AngelScript::asIScriptEngine* engine)
{
  int r = 0;

  r = engine->RegisterObjectType("Edge", 0, AngelScript::asOBJ_REF);AngelScriptCheck(r);

  base_of<Graph>::type::registerAsBaseOfClass<Edge>(engine, "Edge");
}

void Node::registerObjectType(AngelScript::asIScriptEngine* engine)
{
  int r = 0;

  r = engine->RegisterObjectType("Node", 0, AngelScript::asOBJ_REF);AngelScriptCheck(r);

  base_of<Node>::type::registerAsBaseOfClass<Node>(engine, "Node");
}

void test_references()
{
}



int main(int argc, char** argv)
{
  QApplication application(argc, argv);
  AngelScript::asIScriptEngine* engine = AngelScript::asCreateScriptEngine();
  engine->SetEngineProperty(AngelScript::asEP_REQUIRE_ENUM_SCOPE, 1);
  AngelScriptIntegration::init_message_callback_qt(engine);


  asDWORD previousMask = engine->SetDefaultAccessMask(AngelScriptIntegration::ACCESS_MASK_ALL);
  AngelScript::RegisterStdString(engine);
  AngelScript::RegisterScriptArray(engine, true);
  engine->SetDefaultAccessMask(previousMask);

  Graph::registerObjectType(engine);
  Node::registerObjectType(engine);
  Edge::registerObjectType(engine);

  test_references();

  Q_UNUSED(application);
  return 0;
}
