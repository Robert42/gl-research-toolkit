#ifndef GLRT_SCENE_RESOURCES_LIGHTSOURCE_H
#define GLRT_SCENE_RESOURCES_LIGHTSOURCE_H

#include <glrt/scene/coord-frame.h>
#include <glrt/scene/declarations.h>

namespace glrt {
namespace scene {
namespace resources {


struct LightSource
{
  enum class Interactivity
  {
    STATIC,
    DYNAMIC,
    MOVABLE
  };

  enum class Type
  {
    SPHERE_AREA_LIGHT,
    RECT_AREA_LIGHT,
  };

  struct AreaLightCommon
  {
    glm::vec3 color;
    float luminous_power;
    glm::vec3 origin;
    float influence_radius;

    friend AreaLightCommon operator*(const CoordFrame& t, AreaLightCommon lightSource)
    {
      lightSource.origin = t.transform_point(lightSource.origin);
      return lightSource;
    }
  };
  static_assert(sizeof(AreaLightCommon)==32, "Please make sure the struct LightSource is std140 compatible");

  struct SphereAreaLight
  {
    AreaLightCommon areaLightCommon;

    float radius;
    padding<float, 3> _padding;

    friend SphereAreaLight operator*(const CoordFrame& t, SphereAreaLight data)
    {
      data.areaLightCommon = t * data.areaLightCommon;

      return data;
    }
  };
  static_assert(sizeof(SphereAreaLight)==48, "Please make sure the struct SphereAreaLightComponent::Data is std140 compatible");

  struct RectAreaLight
  {
    AreaLightCommon areaLightCommon;

    glm::vec3 tangent1 = glm::vec3(-1, 0, 0);
    float half_width;
    glm::vec3 tangent2 = glm::vec3(0, 1, 0);
    float half_height;

    friend RectAreaLight operator*(const CoordFrame& frame, RectAreaLight data)
    {
      data.areaLightCommon = frame * data.areaLightCommon;
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

  LightComponent* createLightComponent(Entity& entity, const Uuid<LightComponent>& uuid, Interactivity interactivity);

  static void registerAngelScriptTypes();
};


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_LIGHTSOURCE_H
