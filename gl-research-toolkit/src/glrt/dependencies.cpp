#include <glrt/dependencies.h>


template<typename T>
inline uint qHash_for_enum(const T& value, uint seed)
{
  return qHash(static_cast<typename std::underlying_type<T>::type>(value), seed);
}

namespace gl {

uint qHash(const gl::ShaderObject::ShaderType& value, uint seed)
{
  return qHash_for_enum(value, seed);
}

} // namespace gl


namespace glrt {

Exception::Exception(const char* file, int line, const char* function, const QString& message)
{
  qCritical() << function << ": " << message << "\n(in " << file << " line:" << line << ")";
  abort();
}

} // namespace glrt
