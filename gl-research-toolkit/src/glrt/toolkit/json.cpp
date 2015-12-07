#include <glrt/toolkit/json.h>

#include <QJsonArray>

namespace glrt {


bool as_vec3(glm::vec3& vec, const QJsonValue& value, const char* context)
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

glm::vec3 as_vec3_with_fallback(const QJsonValue& value, const glm::vec3& fallback, const char* context)
{
  if(value.isNull())
    return fallback;

  glm::vec3 result;

  if(!as_vec3(result, value, context))
    return fallback;

  return result;
}


bool as_vec2(glm::vec2& vec, const QJsonValue& value, const char* context)
{
  if(!value.isArray())
  {
    qWarning() << context << " parsing vec2 failed";
    return false;
  }

  QJsonArray array = value.toArray();

  if(array.size() != 2 || !array[0].isDouble() || !array[1].isDouble())
  {
    qWarning() << context << " parsing vec2 failed";
    return false;
  }

  vec[0] = array[0].toDouble();
  vec[1] = array[1].toDouble();

  return true;
}

glm::vec2 as_vec2_with_fallback(const QJsonValue& value, const glm::vec2& fallback, const char* context)
{
  if(value.isNull())
    return fallback;

  glm::vec2 result;

  if(!as_vec2(result, value, context))
    return fallback;

  return result;
}


} // namespace glrt

