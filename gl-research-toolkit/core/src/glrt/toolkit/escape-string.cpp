#include <glrt/toolkit/escape-string.h>

namespace glrt {

QString escape_angelscript_string(QString str)
{
  str.replace('\\', "\\\\");
  str.replace('\'', "\\'");
  str.replace('\"', "\\\"");
  str.replace('\n', "\\n");
  str.replace('\r', "\\r");
  str.replace('\t', "\\t");
  return str;
}

QString format_angelscript_vec3(const glm::vec3& v)
{
  return QString("vec3(%0, %1, %2)").arg(v.x).arg(v.y).arg(v.z);
}

} // namespace glrt

