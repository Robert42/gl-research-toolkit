#ifndef GLRT_SCENE_RESOURCES_MATERIAL_H
#define GLRT_SCENE_RESOURCES_MATERIAL_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/uuid.h>

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
};

int qHash(Material::Type materialType);

} // namespace resources
} // namespace scene
} // namespace glrt

#include "material.inl"

#endif // GLRT_SCENE_RESOURCES_MATERIAL_H
