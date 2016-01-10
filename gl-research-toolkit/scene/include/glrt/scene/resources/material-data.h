#ifndef GLRT_SCENE_RESOURCES_MATERIALDATA_H
#define GLRT_SCENE_RESOURCES_MATERIALDATA_H

#include <glrt/dependencies.h>

namespace glrt {
namespace scene {
namespace resources {


struct PlainColorMaterialData
{
  glm::vec3 base_color = glm::vec3(1, 1, 1);
  float smoothness = 0.2f;
  glm::vec3 emission = glm::vec3(0);
  float metal_mask = 0.f;
};

struct MaterialData
{
  enum class Type
  {
    PLAIN_COLOR,
    TEXTURED_OPAQUE,
    TEXTURED_MASKED,
    TEXTURED_TRANSPARENT
  };

  union
  {
    PlainColorMaterialData plainColor;
  };

  Type type;
};


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_MATERIALDATA_H
