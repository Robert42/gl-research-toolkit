#ifndef GLRT_SCENE_LIGHTCOMPONENT_H
#define GLRT_SCENE_LIGHTCOMPONENT_H

#include <glrt/scene/entity.h>
#include <glrt/toolkit/geometry.h>

namespace glrt {
namespace scene {


struct LightSource
{
  glm::vec3 color;
  float luminance;
  glm::vec3 origin;
  float influence_radius;

  friend LightSource operator*(const glm::mat4& t, LightSource lightSource)
  {
    lightSource.origin = transform_point(t, lightSource.origin);
    return lightSource;
  }

  bool initFromJson(QJsonObject& json);
};
static_assert(sizeof(LightSource)==32, "Please make sure the struct LightSource is std140 compatible");


class SphereAreaLightComponent : public VisibleComponent
{
  Q_OBJECT
public:
  struct Data
  {
    LightSource light;

    float radius;
    padding<float, 3> _padding;

    friend Data operator*(const glm::mat4& t, Data data)
    {
      data.light = t * data.light;

      return data;
    }

    bool initFromJson(QJsonObject& json);
  };
  static_assert(sizeof(SphereAreaLightComponent::Data)==48, "Please make sure the struct SphereAreaLightComponent::Data is std140 compatible");

  Data data;

  SphereAreaLightComponent(Entity& entity, const Data& data);
};


class RectAreaLightComponent : public VisibleComponent
{
  Q_OBJECT
public:
  struct Data
  {
    LightSource light;

    glm::vec3 tangent1 = glm::vec3(-1, 0, 0);
    float half_width;
    glm::vec3 tangent2 = glm::vec3(0, 1, 0);
    float half_height;

    friend Data operator*(const glm::mat4& t, Data data)
    {
      data.light = t * data.light;
      data.tangent1 = transform_direction(t, data.tangent1);
      data.tangent2 = transform_direction(t, data.tangent2);

      return data;
    }

    bool initFromJson(QJsonObject& json);
  };
  static_assert(sizeof(RectAreaLightComponent::Data)==64, "Please make sure the struct RectAreaLightComponent::Data is std140 compatible");

  Data data;

  RectAreaLightComponent(Entity& entity, const Data& data);
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_LIGHTCOMPONENT_H
