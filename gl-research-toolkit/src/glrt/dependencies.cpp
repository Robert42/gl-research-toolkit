#include <glrt/dependencies.h>

namespace glrt {

Exception::Exception(const char* file, int line, const char* function, const QString& message)
{
  qCritical() << "Exception thrown: " << message << "\n("<<function << " in " << file << " line:" << line << ")";
  abort();
}

} // namespace glrt

