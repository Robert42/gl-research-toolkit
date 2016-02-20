#ifndef GLRT_SCENE_RESOURCES_MATERIAL_H
#define GLRT_SCENE_RESOURCES_MATERIAL_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/uuid.h>

#include <glhelper/gl.hpp>

namespace glrt {
namespace scene {
namespace resources {

class Material
{
public:
  enum class Type
  {
    PLAIN_COLOR,
    TEXTURED_OPAQUE,
    TEXTURED_MASKED,
    TEXTURED_TRANSPARENT
  };

  struct PlainColor
  {
    glm::vec3 base_color = glm::vec3(1);
    float smoothness = 0.2f;
    glm::vec3 emission = glm::vec3(0);
    float metal_mask = 0.f;

    PlainColor(){}
  };

  struct Textured
  {
    glm::vec4 tint;
    glm::vec2 smoothness_range;
    glm::vec2 occlusion_range;
    glm::vec2 reflectance_range;
    float emission_factor;
    padding<float, 1> _padding;
    GLuint64 diffuse_map;
    GLuint64 normal_map;
    GLuint64 height_map;
    GLuint64 srmo_map; // smoothness, reflectance, metalic_map, occlusion
    GLuint64 emission_map;
  };

  union
  {
    PlainColor plainColor;
  };
  Type type;
  UuidIndex materialUser;

  template<typename T>
  void addMaterialUser(const Uuid<T>& uuid);

  Material(const PlainColor& plainColor = PlainColor());

  const void* data() const;

  static void registerAngelScriptTypes();

  bool operator<(const Material& other) const; // #TODO use this or remove this
};

int qHash(Material::Type materialType);

} // namespace resources
} // namespace scene
} // namespace glrt

#include "material.inl"

#endif // GLRT_SCENE_RESOURCES_MATERIAL_H
