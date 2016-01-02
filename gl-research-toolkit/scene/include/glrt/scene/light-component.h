#ifndef GLRT_SCENE_LIGHTCOMPONENT_H
#define GLRT_SCENE_LIGHTCOMPONENT_H

#include <glrt/scene/entity.h>
#include <glrt/toolkit/geometry.h>

namespace glrt {
namespace scene {


struct LightSource
{
  glm::vec3 color;
  float luminous_power;
  glm::vec3 origin;
  float influence_radius;

  friend LightSource operator*(const CoordFrame& t, LightSource lightSource)
  {
    lightSource.origin = t.transform_point(lightSource.origin);
    return lightSource;
  }

  bool initFromJson(QJsonObject& json);
};
static_assert(sizeof(LightSource)==32, "Please make sure the struct LightSource is std140 compatible");


class SphereAreaLightComponent : public Entity::Component
{
public:
  struct Data
  {
    LightSource light;

    float radius;
    padding<float, 3> _padding;

    friend Data operator*(const CoordFrame& t, Data data)
    {
      data.light = t * data.light;

      return data;
    }

    bool initFromJson(QJsonObject& json);
  };
  static_assert(sizeof(SphereAreaLightComponent::Data)==48, "Please make sure the struct SphereAreaLightComponent::Data is std140 compatible");

  Data data;

  const bool isStatic : 1;

  SphereAreaLightComponent(const Uuid<SphereAreaLightComponent>& uuid, const Data& data, bool isStatic, bool isMovable);
};


class RectAreaLightComponent : public Entity::Component
{
public:
  struct Data
  {
    LightSource light;

    glm::vec3 tangent1 = glm::vec3(-1, 0, 0);
    float half_width;
    glm::vec3 tangent2 = glm::vec3(0, 1, 0);
    float half_height;

    friend Data operator*(const CoordFrame& frame, Data data)
    {
      data.light = frame * data.light;
      data.tangent1 = frame.transform_direction(data.tangent1);
      data.tangent2 = frame.transform_direction(data.tangent2);

      return data;
    }

    bool initFromJson(QJsonObject& json);
  };
  static_assert(sizeof(RectAreaLightComponent::Data)==64, "Please make sure the struct RectAreaLightComponent::Data is std140 compatible");

  Data data;

  RectAreaLightComponent(const Uuid<RectAreaLightComponent>& uuid, const Data& data, bool isMovable);
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_LIGHTCOMPONENT_H
