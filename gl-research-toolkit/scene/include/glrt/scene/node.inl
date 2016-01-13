#ifndef GLRT_SCENE_ENTITY_INL
#define GLRT_SCENE_ENTITY_INL

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

  r = engine->RegisterObjectMethod(className, "NodeComponent@ opImplConv()", AngelScript::asFUNCTION((AngelScriptIntegration::wrap_static_cast<Component,T>)), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptIntegration::AngelScriptCheck(r);
  r = engine->RegisterObjectMethod("NodeComponent", (std::string(className)+"@ opConv()").c_str(), AngelScript::asFUNCTION((AngelScriptIntegration::wrap_static_cast<T, Component>)), AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptIntegration::AngelScriptCheck(r);
}


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_ENTITY_INL
