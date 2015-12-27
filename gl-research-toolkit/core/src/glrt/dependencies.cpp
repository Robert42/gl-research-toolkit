#include <glrt/dependencies.h>


QDebug operator<<(QDebug d, const glm::bvec2& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::bvec3& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::bvec4& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::vec2& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::vec3& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::vec4& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::ivec2& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::ivec3& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::ivec4& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::mat2& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::mat3& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::mat4& v)
{
  return d << glm::to_string(v).c_str();
}


namespace glrt {

Exception::Exception(const char* file, int line, const char* function, const QString& message)
{
  qCritical() << function << ": " << message << "\n(in " << file << " line:" << line << ")";
  abort();
}

AngelScript::asIScriptEngine* angelScriptEngine = nullptr;

} // namespace glrt
