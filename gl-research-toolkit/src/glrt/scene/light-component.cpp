#include <glrt/scene/light-component.h>
#include <glrt/toolkit/json.h>
#include <QJsonObject>

namespace glrt {
namespace scene {


bool LightSource::initFromJson(QJsonObject& json)
{
  this->luminance = json.contains("luminance") ? json["luminance"].toDouble() : 10.0;
  this->color = as_vec3_with_fallback(json["color"], glm::vec3(1), "SphereAreaLightComponent::Data::initFromJson");

  return true;
}


// =============================================================================


SphereAreaLightComponent::SphereAreaLightComponent(Entity& entity, const Data& data)
  : VisibleComponent(entity, false),
    data(data)
{
}


bool SphereAreaLightComponent::Data::initFromJson(QJsonObject& json)
{
  if(!light.initFromJson(json))
    return false;

  this->origin = as_vec3_with_fallback(json["origin"], glm::vec3(0), "SphereAreaLightComponent::Data::initFromJson");
  this->radius = json.contains("radius") ? json["radius"].toDouble() : 1.0;

  return true;
}


// =============================================================================


RectAreaLightComponent::RectAreaLightComponent(Entity& entity, const Data& data)
  : VisibleComponent(entity, false),
    data(data)
{
}


bool RectAreaLightComponent::Data::initFromJson(QJsonObject& json)
{
  if(!light.initFromJson(json))
    return false;

  this->origin = as_vec3_with_fallback(json["origin"], glm::vec3(0), "RectAreaLightComponent::Data::initFromJson");
  this->normal = as_vec3_with_fallback(json["normal"], glm::vec3(0, 0, -1), "RectAreaLightComponent::Data::initFromJson");
  this->tangent = as_vec3_with_fallback(json["tangent"], glm::vec3(-1, 0, 0), "RectAreaLightComponent::Data::initFromJson");
  this->halfWidth = json.contains("width") ? json["width"].toDouble()*.5 : .5;
  this->halfHeight = json.contains("height") ? json["height"].toDouble()*.5 : .5;

  return true;
}


} // namespace scene
} // namespace glrt

