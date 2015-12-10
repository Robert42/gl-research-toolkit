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

  bool initFromJson(QJsonObject& json);
};


class SphereAreaLightComponent : public VisibleComponent
{
  Q_OBJECT
public:
  struct Data
  {
    LightSource light;

    glm::vec3 origin;
    float radius;

    friend Data operator*(const glm::mat4& t, Data data)
    {
      data.origin = transform_point(t, data.origin);

      return data;
    }

    bool initFromJson(QJsonObject& json);
  };
  static_assert(sizeof(SphereAreaLightComponent::Data)==32, "Please make sure the struct SphereAreaLightComponent::Data is std140 compatible");

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

    glm::vec3 normal = glm::vec3(0, 0, -1);
    float half_width;
    glm::vec3 tangent = glm::vec3(-1, 0, 0);
    float half_height;
    glm::vec3 origin = glm::vec3(0);
    padding<float> _padding;

    friend Data operator*(const glm::mat4& t, Data data)
    {
      data.normal = transform_direction(t, data.normal);
      data.tangent = transform_direction(t, data.tangent);
      data.origin = transform_point(t, data.origin);

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
