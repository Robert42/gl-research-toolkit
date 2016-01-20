#ifndef GLRT_RENDERER_MATERIAL_H
#define GLRT_RENDERER_MATERIAL_H

#include <glrt/dependencies.h>

#include <glrt/scene/resources/material.h>

#include <glhelper/buffer.hpp>

namespace glrt {
namespace renderer {

class MaterialInstance : public QObject
{
  Q_OBJECT
public:
  typedef QSharedPointer<MaterialInstance> Ptr;
  typedef scene::resources::Material::Type Type;

  const Type type;
  QString name;
  gl::Buffer uniformBuffer;

  static MaterialInstance::Ptr createDummyMaterial();

private:
  friend class PlainColorMaterial;
  MaterialInstance(const Type type, gl::Buffer&& buffer);
};


class PlainColorMaterial : public MaterialInstance
{
  Q_OBJECT
public:
  typedef glrt::scene::resources::Material::PlainColor UniformData;

  PlainColorMaterial(const UniformData& data);
};


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SCENE_MATERIAL_H
