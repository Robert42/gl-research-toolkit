#ifndef GLRT_RENDERER_SYNCED_FRAGMENTED_COMPONENT_ARRAY_H
#define GLRT_RENDERER_SYNCED_FRAGMENTED_COMPONENT_ARRAY_H

#include <glrt/scene/scene.h>

namespace glrt {
namespace renderer {

template<typename T_component, typename T_FragmentedArray>
class SyncedFragmentedComponentArray : public QObject
{
public:
  T_FragmentedArray fragmented_array;
  quint32 numberVisibleComponents = 0;
  scene::Scene& scene;
  bool dirty : 1;

  SyncedFragmentedComponentArray(scene::Scene& scene);

  void addComponent(T_component* component);

private:
  void handleDeletedComponents(QObject* object);
  void handleVisibilityComponents(glrt::scene::Node::Component* component);
};

} // namespace renderer
} // namespace glrt

#include "synced-fragmented-component-array.inl"

#endif // GLRT_RENDERER_SYNCED_FRAGMENTED_COMPONENT_ARRAY_H
