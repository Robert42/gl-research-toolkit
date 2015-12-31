#ifndef GLRT_RENDERER_MATERIAL_H
#define GLRT_RENDERER_MATERIAL_H

#include <glrt/dependencies.h>

#include <glrt/scene/resources/material-data.h>

#include <glhelper/buffer.hpp>

#include <QJsonObject>

namespace glrt {
namespace renderer {

class MaterialInstance : public QObject
{
  Q_OBJECT
public:
  typedef QSharedPointer<MaterialInstance> Ptr;
  typedef scene::resources::MaterialData::Type Type;

  const Type type;
  QString name;
  gl::Buffer uniformBuffer;

  static MaterialInstance::Ptr fromJson(const QDir& dir, const QJsonObject& object);
  static MaterialInstance::Ptr createDummyMaterial();

protected:
  bool parseCommonJson(const QJsonObject& object, const QString& expectedType);

private:
  friend class PlainColorMaterial;
  MaterialInstance(const Type type, gl::Buffer&& buffer);
};


class PlainColorMaterial : public MaterialInstance
{
  Q_OBJECT
public:
  typedef glrt::scene::resources::PlainColorMaterialData UniformData;

  PlainColorMaterial(const UniformData& data);

  static MaterialInstance::Ptr fromJson(const QDir& dir, const QJsonObject& object);
};


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SCENE_MATERIAL_H
