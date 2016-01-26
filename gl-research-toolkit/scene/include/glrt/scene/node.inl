#ifndef GLRT_SCENE_NODE_INL
#define GLRT_SCENE_NODE_INL

#include "node.h"


namespace glrt {
namespace scene {


template<typename T>
QVector<T*> Node::allModularAttributeWithType(const std::function<bool(T*)>& filter) const
{
  QVector<T*> result;
  result.reserve(this->_allModularAttributes.length());

  for(ModularAttribute* a : this->_allModularAttributes)
    if(is_instance_of<T>(a) && filter(static_cast<T*>(a)))
      result.append(static_cast<T*>(a));

  return result;
}

template<typename T>
QVector<T*> Node::allComponentsWithType(const std::function<bool(T*)>& filter) const
{
  QVector<Component*> allComponents = this->allComponents();

  QVector<T*> result;
  result.reserve(allComponents.length());

  for(Component* c : allComponents)
    if(is_instance_of<T>(c) && filter(static_cast<T*>(c)))
      result.append(static_cast<T*>(c));

  return result;
}


template<typename T>
void Node::Component::registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className)
{
  int r;
  r = engine->RegisterObjectMethod(className, "CoordFrame get_localTransformation()", AngelScript::asMETHOD(Component, localCoordFrame), AngelScript::asCALL_THISCALL);
  AngelScriptIntegration::AngelScriptCheck(r);

  r = engine->RegisterObjectMethod(className, "void set_localTransformation(const CoordFrame &in coordFrame)", AngelScript::asMETHOD(Component, set_localCoordFrame), AngelScript::asCALL_THISCALL);
  AngelScriptIntegration::AngelScriptCheck(r);

  if(!std::is_same<T, Component>::value)
  {
    r = engine->RegisterObjectMethod(className, "NodeComponent@ opImplConv()", AngelScript::asFUNCTION((AngelScriptIntegration::wrap_static_cast<Component,T>)), AngelScript::asCALL_CDECL_OBJFIRST);
    AngelScriptIntegration::AngelScriptCheck(r);
    r = engine->RegisterObjectMethod("NodeComponent", (std::string(className)+"@ opConv()").c_str(), AngelScript::asFUNCTION((AngelScriptIntegration::wrap_static_cast<T, Component>)), AngelScript::asCALL_CDECL_OBJFIRST);
    AngelScriptIntegration::AngelScriptCheck(r);

    // #TODO add also conversion for the uuids
  }
}


template<typename T_Component, typename... T_Args>
struct Node::Component::_create_method_helper<T_Component*(Node& node, Node::Component* parent, T_Args...)>
{
  template<typename T, T* function>
  struct function_wrapper
  {
    static T_Component* createWithNode(Node* node, T_Args... args)
    {
      if(node == nullptr)
        throw GLRT_EXCEPTION("not expected nullptr as parent for creating a NodeComponent");
      return function(*node, nullptr, args...);
    }

    static T_Component* createWithParentComponent(Node::Component* parent, T_Args... args)
    {
      if(parent == nullptr)
        throw GLRT_EXCEPTION("not expected nullptr as node for creating a NodeComponent");
      return function(parent->node, parent, args...);
    }
  };
};


template<typename T, T* function>
void Node::Component::registerCreateMethod(AngelScript::asIScriptEngine* engine, const char* type, const char* arguments)
{
  _registerCreateMethod<T, function>(engine, type, std::string("new_") + type, arguments);
}

template<typename T, T* function>
void Node::Component::_registerCreateMethod(AngelScript::asIScriptEngine* engine, const char* type, const std::string& function_name, const char* arguments)
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  typedef _create_method_helper<T> helper;
  typedef typename helper::template function_wrapper<T, function> wrapper;

  int r;
  r = engine->RegisterGlobalFunction((std::string(type)+"@ "+function_name+"(Node@ node, "+arguments+")").c_str(),
                                     AngelScript::asFUNCTION(wrapper::createWithNode),
                                     AngelScript::asCALL_CDECL);
  AngelScriptIntegration::AngelScriptCheck(r);
  r = engine->RegisterGlobalFunction((std::string(type)+"@ "+function_name+"(NodeComponent@ parent, "+arguments+")").c_str(),
                                     AngelScript::asFUNCTION(wrapper::createWithParentComponent),
                                     AngelScript::asCALL_CDECL);
  AngelScriptIntegration::AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_NODE_INL
