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

  enum class TypeFlag : quint32
  {
    OPAQUE = 0x0,
    PLAIN_COLOR = 0x1,
    TEXTURED = 0x2,

    AREA_LIGHT = 0x01000000 | Material::TypeFlag::OPAQUE | Material::TypeFlag::PLAIN_COLOR,
    SPHERE_LIGHT = 0x02000000 | AREA_LIGHT,
    RECT_LIGHT   = 0x04000000 | AREA_LIGHT,
    
    VERTEX_SHADER_UNIFORM   = 0x08000000,
    FRAGMENT_SHADER_UNIFORM   = 0x10000000,
    TWO_SIDED = 0x20000000,
    MASKED = 0x40000000,
    TRANSPARENT = 0x80000000
  };

  Q_DECLARE_FLAGS(Type, TypeFlag)

  static QString typeToString(Type type);

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
    glm::vec4 tint = glm::vec4(1);
    glm::vec4 srmo_range_0 = glm::vec4(0);
    glm::vec4 srmo_range_1 = glm::vec4(1);
    float emission_factor = 1.f;
    Type type = TypeFlag::TEXTURED; // for T_textureHandle==GLuint64, this is ignored
    T_textureHandle basecolor_map;
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
  TextureHandleType textureHandleType = TextureHandleType::Ids;
  UuidIndex materialUser;

  template<typename T>
  void addMaterialUser(const Uuid<T>& uuid);

  Material(const PlainColor& plainColor = PlainColor(), Type type=TypeFlag::PLAIN_COLOR);
  Material(const Textured<TextureHandle>& textured, Type type);

  const void* data() const;

  void prepareForGpuBuffer();
  bool isTextureType() const;

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

Q_DECLARE_OPERATORS_FOR_FLAGS(glrt::scene::resources::Material::Type);

#include "material.inl"

#endif // GLRT_SCENE_RESOURCES_MATERIAL_H
