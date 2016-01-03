#ifndef GLRT_SCENE_ENTITY_INL
#define GLRT_SCENE_ENTITY_INL

#include "entity.h"

namespace glrt {
namespace scene {


template<typename T>
void Entity::registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className)
{
  static_assert(std::is_base_of<Object, T>::value, "T must inherit from glrt::scene::Entity");

  int r = 0;

  base_of<Entity>::type::registerAsBaseOfClass<T>(engine, className);
}

template<typename T>
void Entity::ModularAttribute::registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className)
{
  static_assert(std::is_base_of<Object, T>::value, "T must inherit from glrt::scene::Entity::ModularAttribute");

  int r = 0;

  base_of<Entity::ModularAttribute>::type::registerAsBaseOfClass<T>(engine, className);

  r = engine->RegisterObjectMethod(className, "void set_entity(Entity@)", asMETHOD_WITH_REF(Entity::ModularAttribute, set_entity), AngelScript::asCALL_THISCALL);
  r = engine->RegisterObjectMethod(className, "Entity@ get_entity()", asMETHOD_WITH_REF(Entity::ModularAttribute, get_entity), AngelScript::asCALL_THISCALL);
}

template<typename T>
void Entity::Component::registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className)
{
  static_assert(std::is_base_of<Object, T>::value, "T must inherit from glrt::scene::Entity::Component");

  int r = 0;

  base_of<Entity::Component>::type::registerAsBaseOfClass<T>(engine, className);
}

// ========

template<typename T>
QVector<T*> Entity::allModularAttributeWithType(const std::function<bool(T*)>& filter) const
{
  QVector<T*> result;
  result.reserve(this->_allModularAttributes.length());

  for(ModularAttribute* a : this->_allModularAttributes)
    if(is_instance_of<T>(a) && filter(static_cast<T*>(a)))
      result.append(static_cast<T*>(a));

  return result;
}

template<typename T>
QVector<T*> Entity::allComponentsWithType(const std::function<bool(T*)>& filter) const
{
  //QVector<Component*> allComponents = this->allComponents(); // #IMPLEMENT!!!!!!!!!!!
  QVector<Component*> allComponents; // this is dummy code to be removed

  QVector<T*> result;
  result.reserve(allComponents.length());

  for(Component* c : allComponents)
    if(is_instance_of<T>(c) && filter(static_cast<T*>(c)))
      result.append(static_cast<T*>(c));

  return result;
}


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_ENTITY_INL
