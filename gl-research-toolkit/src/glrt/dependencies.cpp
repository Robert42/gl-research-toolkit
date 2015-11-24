#include <glrt/dependencies.h>

namespace glrt {

Exception::Exception(const char* file, int line, const char* function, const QString& message)
{
  qCritical() << function << ": " << message << "\n( in " << file << " line:" << line << ")";
  abort();
}

} // namespace glrt

