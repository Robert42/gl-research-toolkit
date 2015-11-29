#ifndef GLRT_SCENE_MATERIAL_H
#define GLRT_SCENE_MATERIAL_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>

namespace glrt {
namespace scene {

class Material : public QObject
{
  Q_OBJECT
public:
  typedef QSharedPointer<Material> Ptr;

  gl::Buffer uniformBuffer;

  Material(gl::Buffer&& buffer);
};


class PlainColorMaterial : public Material
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
