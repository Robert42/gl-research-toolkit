#ifndef GLRT_SCENE_RESOURCES_MATERIAL_H
#define GLRT_SCENE_RESOURCES_MATERIAL_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/uuid.h>
#include <glrt/toolkit/array.h>
#include <glrt/scene/resources/texture-manager.h>

#include <glhelper/gl.hpp>

namespace glrt {
namespace scene {
namespace resources {

class Material
{
public:
  typedef TextureManager::TextureHandle TextureHandle;

  enum class Type
  {
    PLAIN_COLOR,
    TEXTURED_OPAQUE,
    TEXTURED_MASKED,
    TEXTURED_TRANSPARENT
  };

  static QVector<Type> allTypes();

  struct PlainColor
  {
    glm::vec3 base_color = glm::vec3(1);
    float smoothness = 0.2f;
    glm::vec3 emission = glm::vec3(0);
    float metal_mask = 0.f;

    PlainColor(){}
  };

  template<typename T_textureHandle>
  struct Textured
  {
    glm::vec4 tint;
    glm::vec2 smoothness_range;
    glm::vec2 occlusion_range;
    glm::vec2 reflectance_range;
    float emission_factor;
    padding<float, 1> _padding;
    T_textureHandle diffuse_map;
    T_textureHandle normal_map;
    T_textureHandle height_map;
    T_textureHandle srmo_map; // smoothness, reflectance, metalic_map, occlusion
    T_textureHandle emission_map;
  };

  enum class TextureHandleType
  {
    Ids,
    GpuPtrs
  };

  union
  {
    PlainColor plainColor;
    Textured<TextureHandle> texturesIds;
    Textured<GLuint64> textureGpuPtrs;
  };
  Type type;
  TextureHandleType textureHandleType = TextureHandleType::Ids; // #TODO: make sure to convert the material before passing it to as material uniform
  UuidIndex materialUser;

  template<typename T>
  void addMaterialUser(const Uuid<T>& uuid);

  Material(const PlainColor& plainColor = PlainColor());

  const void* data() const;

  static void registerAngelScriptTypes();

  bool operator<(const Material& other) const;
};

int qHash(Material::Type materialType);

} // namespace resources
} // namespace scene


template<>
struct DefaultTraits<glrt::scene::resources::Material>
{
  typedef ArrayTraits_POD<glrt::scene::resources::Material> type;
};

template<>
struct DefaultTraits<glrt::scene::resources::Material::Type>
{
  typedef ArrayTraits_POD<glrt::scene::resources::Material::Type> type;
};


} // namespace glrt

#include "material.inl"

#endif // GLRT_SCENE_RESOURCES_MATERIAL_H
