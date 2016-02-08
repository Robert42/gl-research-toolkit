#ifndef GLRT_SCENE_COLLECTSCENEDATA_H
#define GLRT_SCENE_COLLECTSCENEDATA_H

#include <glrt/scene/scene.h>

#include <glrt/scene/camera-component.h>
#include <glrt/scene/light-component.h>

namespace glrt {
namespace scene {

template<typename T_component>
QVector<T_component*> collectAllComponentsWithType(Scene* scene, const std::function<bool(T_component*)>& filter=always_return_true<T_component*>);



template<typename T_component, typename T_data>
QVector<T_data> collectData(Scene* scene, const std::function<T_data(T_component*)>& get_data);

template<typename T_component, typename T_data>
QHash<QString, T_data> collectNamedData(Scene* scene, const std::function<T_data(T_component*)>& get_data);


QVector<CameraParameter> collectCameraParameters(Scene* scene);
QHash<QString, CameraParameter> collectNamedCameraParameters(Scene* scene);

QVector<SphereAreaLightComponent::Data> collectSphereAreaLights(Scene* scene);
QHash<QString, scene::SphereAreaLightComponent::Data> collectNamedSphereAreaLights(Scene* scene);

QVector<RectAreaLightComponent::Data> collectRectAreaLights(Scene* scene);
QHash<QString, RectAreaLightComponent::Data> collectNamedRectAreaLights(Scene* scene);


} // namespace scene
} // namespace glrt

#include "collect-scene-data.inl"

#endif // GLRT_SCENE_COLLECTSCENEDATA_H
