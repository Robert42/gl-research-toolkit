#ifndef GLRT_SCENE_MATERIAL_H
#define GLRT_SCENE_MATERIAL_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>

namespace glrt {
namespace scene {

class MaterialInstance : public QObject
{
  Q_OBJECT
public:
  typedef QSharedPointer<MaterialInstance> Ptr;

  enum class Type
  {
    PLAIN_COLOR,
    TEXTURED_OPAQUE,
    TEXTURED_MASKED,
    TEXTURED_TRANSPARENT
  };

  const Type type;
  gl::Buffer uniformBuffer;

private:
  friend class PlainColorMaterial;
  MaterialInstance(const Type type, gl::Buffer&& buffer);
};


class PlainColorMaterial : public MaterialInstance
{
  Q_OBJECT
public:
  struct UniformData
  {
    glm::vec3 diffuse;
    float roughness;
    float metallicness;
  };

  PlainColorMaterial(const UniformData& data);
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_MATERIAL_H
