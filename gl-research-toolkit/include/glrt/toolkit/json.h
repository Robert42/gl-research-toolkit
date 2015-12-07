#ifndef GLRT_JSON_H
#define GLRT_JSON_H

#include <glrt/dependencies.h>

#include <QJsonValue>

namespace glrt {


bool as_vec3(glm::vec3& vec, const QJsonValue& value, const char* context);
glm::vec3 as_vec3_with_fallback(const QJsonValue& value, const glm::vec3& fallback, const char* context);

bool as_vec2(glm::vec2& vec, const QJsonValue& value, const char* context);
glm::vec2 as_vec2_with_fallback(const QJsonValue& value, const glm::vec2& fallback, const char* context);


} // namespace glrt

#endif // GLRT_JSON_H
