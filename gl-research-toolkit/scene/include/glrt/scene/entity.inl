#ifndef GLRT_SCENE_ENTITY_INL
#define GLRT_SCENE_ENTITY_INL

#include "entity.h"

namespace glrt {
namespace scene {


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
