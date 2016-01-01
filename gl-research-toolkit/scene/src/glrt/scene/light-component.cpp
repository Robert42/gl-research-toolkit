#include <glrt/scene/light-component.h>
#include <glrt/toolkit/json.h>
#include <QJsonObject>

namespace glrt {
namespace scene {


bool LightSource::initFromJson(QJsonObject& json)
{
  this->luminous_power = json.contains("luminous_power") ? json["luminous_power"].toDouble() : 25.0;
  this->color = as_vec3_with_fallback(json["color"], glm::vec3(1), "SphereAreaLightComponent::Data::initFromJson");
  this->origin = as_vec3_with_fallback(json["origin"], glm::vec3(0), "SphereAreaLightComponent::Data::initFromJson");
  this->influence_radius = json.contains("influence_radius") ? float(json["influence_radius"].toDouble()) : INFINITY;

  return true;
}


// =============================================================================


SphereAreaLightComponent::SphereAreaLightComponent(const Uuid<SphereAreaLightComponent>& uuid, const Data& data, bool isStatic, bool isMovable)
  : Entity::Component(uuid, isMovable),
    data(data),
    isStatic(isStatic)
{
}


bool SphereAreaLightComponent::Data::initFromJson(QJsonObject& json)
{
  if(!light.initFromJson(json))
    return false;

  this->radius = json.contains("radius") ? json["radius"].toDouble() : 1.0;

  return true;
}


// =============================================================================


RectAreaLightComponent::RectAreaLightComponent(const Uuid<RectAreaLightComponent>& uuid, const Data& data, bool isMovable)
  : Entity::Component(uuid, isMovable),
    data(data)
{
}


bool RectAreaLightComponent::Data::initFromJson(QJsonObject& json)
{
  if(!light.initFromJson(json))
    return false;

  glm::vec3 normal = as_vec3_with_fallback(json["normal"], glm::vec3(0, 0, -1), "RectAreaLightComponent::Data::initFromJson");
  glm::vec3 tangent = as_vec3_with_fallback(json["tangent"], glm::vec3(-1, 0, 0), "RectAreaLightComponent::Data::initFromJson");

  this->tangent1 = tangent;
  this->tangent2 = glm::cross(normal, tangent);
  this->half_width = json.contains("width") ? json["width"].toDouble()*.5 : .5;
  this->half_height = json.contains("height") ? json["height"].toDouble()*.5 : .5;

  return true;
}


} // namespace scene
} // namespace glrt

