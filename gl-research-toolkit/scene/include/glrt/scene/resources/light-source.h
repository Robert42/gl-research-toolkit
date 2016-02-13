#ifndef GLRT_SCENE_RESOURCES_LIGHTSOURCE_H
#define GLRT_SCENE_RESOURCES_LIGHTSOURCE_H

#include <glrt/scene/coord-frame.h>
#include <glrt/scene/declarations.h>
#include <glrt/toolkit/array.h>

namespace glrt {
namespace scene {
namespace resources {


struct LightSource final
{
  enum class Type
  {
    SPHERE_AREA_LIGHT,
    RECT_AREA_LIGHT,
  };

  struct AreaLightCommon final
  {
    glm::vec3 color = glm::vec3(1);
    float luminous_power = 25.f;
    glm::vec3 origin = glm::vec3(0);
    float influence_radius = INFINITY; // #TODO test, whether the influence radius is already working

    AreaLightCommon(){}

    friend AreaLightCommon operator*(const CoordFrame& t, AreaLightCommon lightSource)
    {
      lightSource.origin = t.transform_point(lightSource.origin);
      return lightSource;
    }
  };
  static_assert(sizeof(AreaLightCommon)==32, "Please make sure the struct LightSource is std140 compatible");

  struct SphereAreaLight final
  {
    AreaLightCommon areaLightCommon;

    float radius = 0.01f;
    padding<float, 3> _padding;

    SphereAreaLight(){}

    friend SphereAreaLight operator*(const CoordFrame& t, SphereAreaLight data)
    {
      data.areaLightCommon = t * data.areaLightCommon;
      data.radius = t.scaleFactor * data.radius;

      return data;
    }
  };
  static_assert(sizeof(SphereAreaLight)==48, "Please make sure the struct SphereAreaLightComponent::Data is std140 compatible");

  struct RectAreaLight final
  {
    AreaLightCommon areaLightCommon;

    glm::vec3 tangent1 = glm::vec3(-1, 0, 0);
    float half_width = 1.f;
    glm::vec3 tangent2 = glm::vec3(0, 1, 0);
    float half_height = 1.f;

    friend RectAreaLight operator*(const CoordFrame& frame, RectAreaLight data)
    {
      data.areaLightCommon = frame * data.areaLightCommon;
      data.half_width = frame.scaleFactor * data.half_width;
      data.half_height = frame.scaleFactor * data.half_height;
      data.tangent1 = frame.transform_direction(data.tangent1);
      data.tangent2 = frame.transform_direction(data.tangent2);

      return data;
    }
  };
  static_assert(sizeof(RectAreaLight)==64, "Please make sure the struct RectAreaLightComponent::Data is std140 compatible");


  union
  {
    SphereAreaLight sphere_area_light;
    RectAreaLight rect_area_light;
  };
  Type type;

  LightSource(const RectAreaLight& rectAreaLight);
  LightSource(const SphereAreaLight& sphereAreaLight=SphereAreaLight());

  static void registerAngelScriptTypes();
};


} // namespace resources
} // namespace scene


template<>
struct DefaultTraits<scene::resources::LightSource::SphereAreaLight>
{
  typedef ArrayTraits_Unordered_POD<scene::resources::LightSource::SphereAreaLight> type;
};

template<>
struct DefaultTraits<scene::resources::LightSource::RectAreaLight>
{
  typedef ArrayTraits_Unordered_POD<scene::resources::LightSource::RectAreaLight> type;
};

} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_LIGHTSOURCE_H
