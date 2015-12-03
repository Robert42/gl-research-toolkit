#include <glrt/scene/material.h>

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
  return Ptr(new PlainColorMaterial(PlainColorMaterial::UniformData()));
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


bool MaterialInstance::asVec3(glm::vec3& vec, const QJsonValue& value, const char* context)
{
  if(!value.isArray())
  {
    qWarning() << context << " parsing vec3 failed";
    return false;
  }

  QJsonArray array = value.toArray();

  if(array.size() != 3 || !array[0].isDouble() || !array[1].isDouble() || !array[2].isDouble())
  {
    qWarning() << context << " parsing vec3 failed";
    return false;
  }

  vec[0] = array[0].toDouble();
  vec[1] = array[1].toDouble();
  vec[2] = array[2].toDouble();

  return true;
}


// ======== PlainColorMaterial =================================================


PlainColorMaterial::PlainColorMaterial(const UniformData& data)
  : MaterialInstance(Type::PLAIN_COLOR,
                     gl::Buffer(sizeof(UniformData), gl::Buffer::UsageFlag::IMMUTABLE, &data)) // FIXME: wither use std140 or query the position of each member. See http://stackoverflow.com/a/13036724/2301866
{
}


MaterialInstance::Ptr PlainColorMaterial::fromJson(const QDir&, const QJsonObject& object)
{
  UniformData uniformData;

  if(!asVec3(uniformData.diffuse, object["diffuse"], "PlainColorMaterial::fromJson (diffuse)"))
    return Ptr();
  if(object.contains("emission") && !asVec3(uniformData.emission, object["emission"], "PlainColorMaterial::fromJson (emission)"))
    return Ptr();

  uniformData.metallicness = object["metallic"].toDouble(0);
  uniformData.roughness = object["rougness"].toDouble(0.8);

  PlainColorMaterial* material = new PlainColorMaterial(uniformData);

  if(!material->parseCommonJson(object, "PLAIN_COLOR"))
    return Ptr();

  return Ptr(material);
}


} // namespace scene
} // namespace glrt

