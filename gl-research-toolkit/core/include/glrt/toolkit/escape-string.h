#ifndef GLRT_ESCAPESTRING_H
#define GLRT_ESCAPESTRING_H

#include <glrt/dependencies.h>

namespace glrt {

QString escape_angelscript_string(QString str);
QString format_angelscript_vec3(const glm::vec3& v);

} // namespace glrt

#endif // GLRT_ESCAPESTRING_H
