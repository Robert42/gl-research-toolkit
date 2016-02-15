#ifndef GLRT_SCENE_COLLECTSCENEDATA_INL
#define GLRT_SCENE_COLLECTSCENEDATA_INL

#include "collect-scene-data.h"
#include "resources/resource-manager.h"
#include "scene-layer.h"

namespace glrt {
namespace scene {

template<typename T_component>
Array<T_component*> collectAllComponentsWithType(Scene* scene, const std::function<bool(T_component*)>& filter)
{
  static_assert(std::is_base_of<Node::Component, T_component>::value, "T_component must inherit from Node::Component");

  Array<T_component*> components;
  int totalNumberEntites = 0;
  for(const SceneLayer* layer : scene->allLayers())
    totalNumberEntites += layer->allNodes().length();

  components.reserve(totalNumberEntites / 2);

  for(const SceneLayer* layer : scene->allLayers())
    for(Node* n : layer->allNodes())
      for(T_component* component : n->allComponentsWithType<T_component>(filter))
        components.append(component);

  return components;
}

template<typename T_component>
QHash<QString, T_component*> collectAllComponentsWithTypeNamed(Scene* scene, const std::function<bool(T_component*)>& filter)
{
  static_assert(std::is_base_of<Node::Component, T_component>::value, "T_component must inherit from Node::Component");

  QHash<QString, T_component*> components;

  for(const SceneLayer* layer : scene->allLayers())
    for(Node* n : layer->allNodes())
      for(T_component* component : n->allComponentsWithType<T_component>(filter))
        components.insert(scene->resourceManager.labelForResourceUuid(component->uuid), component);

  return components;
}

template<typename T_att>
Array<T_att*> collectAllModularAttributesWithType(Scene* scene, const std::function<bool(T_att*)>& filter)
{
  static_assert(std::is_base_of<Node::ModularAttribute, T_att>::value, "T_att must inherit from Node::ModularAttribute");

  Array<T_att*> attributes;
  int totalNumberEntites = 0;
  for(const SceneLayer* layer : scene->allLayers())
    totalNumberEntites += layer->allNodes().length();

  attributes.reserve(totalNumberEntites / 2);

  for(const SceneLayer* layer : scene->allLayers())
    for(Node* n : layer->allNodes())
      for(T_att* component : n->allModularAttributeWithType<T_att>(filter))
        attributes.append(component);

  return attributes;
}

template<typename T_component>
T_component* findComponent(Scene* scene, const Uuid<T_component>& uuid)
{
  for(const SceneLayer* layer : scene->allLayers())
    for(Node* n : layer->allNodes())
      for(T_component* component : n->allComponentsWithType<T_component>())
        if(component->uuid == uuid)
          return component;
  return nullptr;
}

template<typename T_component, typename T_data>
QVector<T_data> collectData(Scene* scene, const std::function<T_data(T_component*)>& get_data)
{
  Array<T_component*> components = collectAllComponentsWithType<T_component>(scene);
  QVector<T_data> data;

  data.reserve(components.length());

  for(T_component* component : components)
    data.append(get_data(component));

  return data;
}


template<typename T_component, typename T_data>
QHash<QString, T_data> collectNamedData(Scene* scene, const std::function<T_data(T_component*)>& get_data)
{
  Array<T_component*> components = collectAllComponentsWithType<T_component>(scene);
  QHash<QString, T_data> data;

  data.reserve(components.length());

  for(T_component* component : components)
    data[scene->resourceManager.labelForResourceUuid(component->uuid)] = get_data(component);

  return data;
}


} // namespace scene
} // namespace glrt


#endif // GLRT_SCENE_COLLECTSCENEDATA_INL
