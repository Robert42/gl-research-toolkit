#ifndef GLRT_SCENE_COLLECTSCENEDATA_INL
#define GLRT_SCENE_COLLECTSCENEDATA_INL

#include "collect-scene-data.h"

namespace glrt {
namespace scene {

template<typename T_component>
QVector<T_component*> collectAllComponentsWithType(Scene* scene, bool increaseReferenceCount, const std::function<bool(T_component*)>& filter)
{
  static_assert(std::is_base_of<Entity::Component, T_component>::value, "T_component must inherit from Entity::Component");

  QVector<T_component*> components;
  components.reserve((scene->allEntities().length()+3) / 4);

  if(increaseReferenceCount)
  {
    for(Entity* e : scene->allEntities())
      for(T_component* component : e->allComponentsWithType<T_component>(filter))
      {
        components.append(component);
        component->addReference();
      }
  }else
  {
    for(Entity* e : scene->allEntities())
      for(T_component* component : e->allComponentsWithType<T_component>(filter))
        components.append(component);
  }
  return components;
}

template<typename T_component, typename T_data>
QVector<T_data> collectData(Scene* scene, const std::function<T_data(T_component*)>& get_data)
{
  QVector<T_component*> components = collectAllComponentsWithType<T_component>(scene, true);
  QVector<T_data> data;

  data.reserve(components.length());

  for(T_component* component : components)
  {
    data.append(get_data(component));
    component->releaseReference();
  }

  return data;
}


template<typename T_component, typename T_data>
QHash<QString, T_data> collectNamedData(Scene* scene, const std::function<T_data(T_component*)>& get_data)
{
  QVector<T_component*> components = collectAllComponentsWithType<T_component>(scene, true);
  QHash<QString, T_data> data;

  data.reserve(components.length());

  for(T_component* component : components)
  {
    data[component->get_label()] = get_data(component);
    component->releaseReference();
  }

  return data;
}


} // namespace scene
} // namespace glrt


#endif // GLRT_SCENE_COLLECTSCENEDATA_INL
