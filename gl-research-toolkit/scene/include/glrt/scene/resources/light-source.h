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

  struct CompactAreaLight
  {
    glm::vec3 color = glm::vec3(1);
    float luminous_power = 25.f;
    float influence_radius = INFINITY;

    void swap(CompactAreaLight& other)
    {
      std::swap(other.color, color);
      std::swap(other.luminous_power, luminous_power);
      std::swap(other.influence_radius, influence_radius);
    }
  };

  struct AreaLightCommon final
  {
    glm::vec3 color = glm::vec3(1);
    float luminous_power = 25.f;
    glm::vec3 origin = glm::vec3(0);
    float influence_radius = INFINITY;

    AreaLightCommon(){}

    operator CompactAreaLight() const
    {
      CompactAreaLight l;
      l.color = color;
      l.luminous_power = luminous_power;
      l.influence_radius = influence_radius;
      return l;
    }

    void operator=(const CompactAreaLight& l)
    {
      color = l.color;
      luminous_power = l.luminous_power;
      influence_radius = l.influence_radius;
    }

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

    glm::vec3 normal() const
    {
      const glm::vec3& tangent = tangent1;
      const glm::vec3& bitangent = tangent2;
      return cross(tangent, bitangent);
    }

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
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_LIGHTSOURCE_H
