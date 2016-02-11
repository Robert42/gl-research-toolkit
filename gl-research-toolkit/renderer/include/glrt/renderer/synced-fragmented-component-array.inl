#ifndef GLRT_RENDERER_SYNCED_FRAGMENTED_COMPONENT_ARRAY_INL
#define GLRT_RENDERER_SYNCED_FRAGMENTED_COMPONENT_ARRAY_INL

#include "synced-fragmented-component-array.h"
#include <glrt/scene/collect-scene-data.h>

namespace glrt {
namespace renderer {

template<typename T_component, typename T_FragmentedArray>
SyncedFragmentedComponentArray<T_component, T_FragmentedArray>::SyncedFragmentedComponentArray(scene::Scene& scene)
  : scene(scene),
    dirty(true)
{
  connect(&scene, glrt::scene::ComponentAddedSignal<T_component>::signal(scene),
          this, &SyncedFragmentedComponentArray<T_component, T_FragmentedArray>::addComponent);

  for(T_component* c : glrt::scene::collectAllComponentsWithType<T_component>(scene))
    addComponent(c);
}

template<typename T_component, typename T_FragmentedArray>
void SyncedFragmentedComponentArray<T_component, T_FragmentedArray>::addComponent(T_component* component)
{
#ifdef QT_DEBUG // only connect for a check
  connect(component, &QObject::destroyed,
          this, &SyncedFragmentedComponentArray<T_component, T_FragmentedArray>::handleDeletedComponents);
#endif
  connect(component, &glrt::scene::Node::Component::componentVisibilityChanged,
          this, &SyncedFragmentedComponentArray<T_component, T_FragmentedArray>::handleVisibilityComponents);

  handleVisibilityComponents(component);
}

template<typename T_component, typename T_FragmentedArray>
void SyncedFragmentedComponentArray<T_component, T_FragmentedArray>::handleDeletedComponents(QObject* object)
{
  // the hidden event is guaranteed to be called before deleting the component, so just check, that really nothing has to be done
  Q_ASSERT(fragmented_array.indexOf_Safe(static_cast<T_component*>(object)) == -1);
}

template<typename T_component, typename T_FragmentedArray>
void SyncedFragmentedComponentArray<T_component, T_FragmentedArray>::handleVisibilityComponents(T_component* component)
{
  dirty = true;

  if(component->visible())
  {
    Q_ASSERT(fragmented_array.indexOf(component) == -1);
    fragmented_array.append(component);
  }else
  {
    Q_ASSERT(fragmented_array.indexOf(component) != -1);
    fragmented_array.remove(fragmented_array.indexOf(component));
  }
}


} // namespace renderer
} // namespace glrt


#endif // GLRT_RENDERER_SYNCED_FRAGMENTED_COMPONENT_ARRAY_INL
