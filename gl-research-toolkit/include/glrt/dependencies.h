#ifndef GLRT_DEPENDENCIES_H
#define GLRT_DEPENDENCIES_H

#include <GL/glew.h>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/string_cast.hpp>

#include <AntTweakBar.h>

#include <QDebug>
#include <QString>
#include <QDir>
#include <QObject>
#include <QSharedPointer>

#include <iostream>
#include <memory>
#include <vector>
#include <functional>


QDebug operator<<(QDebug d, const glm::bvec2& v);
QDebug operator<<(QDebug d, const glm::bvec3& v);
QDebug operator<<(QDebug d, const glm::bvec4& v);

QDebug operator<<(QDebug d, const glm::vec2& v);
QDebug operator<<(QDebug d, const glm::vec3& v);
QDebug operator<<(QDebug d, const glm::vec4& v);

QDebug operator<<(QDebug d, const glm::ivec2& v);
QDebug operator<<(QDebug d, const glm::ivec3& v);
QDebug operator<<(QDebug d, const glm::ivec4& v);

QDebug operator<<(QDebug d, const glm::mat2& v);
QDebug operator<<(QDebug d, const glm::mat3& v);
QDebug operator<<(QDebug d, const glm::mat4& v);


namespace glrt {

class Exception final
{
public:
  Exception(const char* file, int line, const char* function, const QString& message);
};

template<typename T, int N=1>
struct padding final
{
private:
  quint8 _[sizeof(T)*N];
};

} // namespace glrt

#define GLRT_EXCEPTION(message) Exception(__FILE__, __LINE__, __FUNCTION__, message)

#define __CALL_SDL(x, action) if(!(x)){qCritical() << #x << " failed: " << SDL_GetError() << "\n("<<__FILE__<<", " << __LINE__ << ")";action}
#define CALL_SDL(x) __CALL_SDL(x,)
#define CALL_SDL_CRITICAL(x) __CALL_SDL(x,abort();)

#define PRINT_VALUE(x) qDebug() << "Value of `" << #x << "` = " << x;

#define TWEAKBAR_BLENDER_AXIS " axisy=-z axisz=y "

#endif // GLRT_DEPENDENCIES_H
