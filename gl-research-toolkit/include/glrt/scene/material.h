#ifndef GLRT_SCENE_MATERIAL_H
#define GLRT_SCENE_MATERIAL_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>

#include <QJsonObject>

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
  QString name;
  gl::Buffer uniformBuffer;

  static MaterialInstance::Ptr fromJson(const QJsonObject& object);

protected:
  bool parseCommonJson(const QJsonObject& object, const QString& expectedType);

  static bool asVec3(glm::vec3& vec, const QJsonValue& value, const char* context);

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
    glm::vec3 diffuse = glm::vec3(1);
    float roughness = 0.8f;
    float metallicness = 0.f;
  };

  PlainColorMaterial(const UniformData& data);

  static MaterialInstance::Ptr fromJson(const QJsonObject& object);
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_MATERIAL_H
