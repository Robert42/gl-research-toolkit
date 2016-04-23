#ifndef GLRT_SCENE_SCENE_INL
#define GLRT_SCENE_SCENE_INL

#include "scene.h"
#include <glrt/scene/declarations.h>

namespace glrt {
namespace scene {
namespace implementation
{

template<class T>
struct ComponentAddedSignal;

template<>
struct ComponentAddedSignal<scene::StaticMeshComponent>
{
  typedef void(Scene::*function_type)(StaticMeshComponent* component);
  static function_type signal(scene::Scene* scene)
  {
    Q_ASSERT(is_instance_of<scene::Scene>(scene));
    return &Scene::StaticMeshComponentAdded;
  }
};

template<>
struct ComponentAddedSignal<LightComponent>
{
  typedef void(Scene::*function_type)(LightComponent* component);
  static function_type signal(scene::Scene* scene)
  {
    Q_ASSERT(is_instance_of<scene::Scene>(scene));
    return &Scene::LightComponentAdded;
  }
};

template<>
struct ComponentAddedSignal<SphereAreaLightComponent>
{
  typedef void(Scene::*function_type)(SphereAreaLightComponent* component);
  static function_type signal(scene::Scene* scene)
  {
    Q_ASSERT(is_instance_of<scene::Scene>(scene));
    return &Scene::SphereAreaLightComponentAdded;
  }
};

template<>
struct ComponentAddedSignal<RectAreaLightComponent>
{
  typedef void(Scene::*function_type)(RectAreaLightComponent* component);
  static function_type signal(scene::Scene* scene)
  {
    Q_ASSERT(is_instance_of<scene::Scene>(scene));
    return &Scene::RectAreaLightComponentAdded;
  }
};

template<>
struct ComponentAddedSignal<VoxelDataComponent>
{
  typedef void(Scene::*function_type)(VoxelDataComponent* component);
  static function_type signal(scene::Scene* scene)
  {
    Q_ASSERT(is_instance_of<scene::Scene>(scene));
    return &Scene::VoxelDataComponentAdded;
  }
};

} // namespace implementation
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENE_INL
