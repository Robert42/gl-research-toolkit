#include <glrt/scene/material.h>
#include <glrt/toolkit/json.h>

#include <QJsonArray>

namespace glrt {
namespace scene {


// ======== Material ===========================================================


MaterialInstance::MaterialInstance(const Type type, gl::Buffer&& buffer)
  : type(type),
    uniformBuffer(std::move(buffer))
{

}


MaterialInstance::Ptr MaterialInstance::fromJson(const QDir& dir, const QJsonObject& object)
{
  if(!object.contains("type"))
  {
    qWarning() << "MaterialInstance::fromJson: material " << object["name"].toString() << "is missing a type";
    return Ptr();
  }

  if(object["type"].toString() == "PLAIN_COLOR")
    return PlainColorMaterial::fromJson(dir, object);

  qWarning() << "MaterialInstance::fromJson: Unkown type " << object["type"].toString() << "is missing a type";
  return Ptr();
}

MaterialInstance::Ptr MaterialInstance::createDummyMaterial()
{
  PlainColorMaterial::UniformData data;

  data.diffuse = glm::vec3(0);
  data.emission = glm::vec3(1, 0, 1);

  return Ptr(new PlainColorMaterial(data));
}

bool MaterialInstance::parseCommonJson(const QJsonObject& object, const QString& expectedType)
{
  if(!object.contains("type"))
  {
    qWarning() << "PlainColorMaterial::fromJson: missing name";
    return false;
  }
  if(object["type"].toString() != expectedType)
  {
    qWarning() << "PlainColorMaterial::fromJson: wrong type " << object["type"].toString() << "  Expected: " << expectedType;
    return false;
  }
  if(!object.contains("name"))
  {
    qWarning() << "PlainColorMaterial::fromJson: missing name";
    return false;
  }
  this->name = object["name"].toString();
  return true;
}


// ======== PlainColorMaterial =================================================


PlainColorMaterial::PlainColorMaterial(const UniformData& data)
  : MaterialInstance(Type::PLAIN_COLOR,
                     gl::Buffer(sizeof(UniformData), gl::Buffer::UsageFlag::IMMUTABLE, &data))
{
}


MaterialInstance::Ptr PlainColorMaterial::fromJson(const QDir&, const QJsonObject& object)
{
  UniformData uniformData;

  if(!as_vec3(uniformData.diffuse, object["diffuse"], "PlainColorMaterial::fromJson (diffuse)"))
    return Ptr();
  if(object.contains("emission") && !as_vec3(uniformData.emission, object["emission"], "PlainColorMaterial::fromJson (emission)"))
    return Ptr();

  uniformData.metallic = object["metallic"].toDouble(0);
  uniformData.roughness = object["roughness"].toDouble(0.8);

  PlainColorMaterial* material = new PlainColorMaterial(uniformData);

  if(!material->parseCommonJson(object, "PLAIN_COLOR"))
    return Ptr();

  return Ptr(material);
}


} // namespace scene
} // namespace glrt

