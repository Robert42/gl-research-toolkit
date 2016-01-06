#ifndef GLRT_DEPENDENCIES_H
#define GLRT_DEPENDENCIES_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/string_cast.hpp>

#include <angelscript.h>
#include <angelscript-integration/ref-counted-object.h>

#include <QDebug>
#include <QString>
#include <QDir>
#include <QObject>
#include <QSharedPointer>
#include <QSet>
#include <QStack>
#include <QUuid>

#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <cstring>

#include <SDL2/SDL.h>


// the unnecessary macros make my syntax highligh unpretty ;)
#ifdef __GNUC__
#ifdef bool
#undef bool
#endif
#ifdef true
#undef true
#endif
#ifdef false
#undef false
#endif
#endif


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

QDebug operator<<(QDebug d, const glm::quat& q);

QDebug operator<<(QDebug d, const std::string& s);


namespace glrt {


using AngelScript::asDWORD;

const asDWORD ACCESS_MASK_RESOURCE_LOADING = AngelScriptIntegration::ACCESS_MASK_USER;
const asDWORD ACCESS_MASK_USER = ACCESS_MASK_RESOURCE_LOADING<<1;

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

extern AngelScript::asIScriptEngine* angelScriptEngine;

template<typename T_parent, typename T_child>
bool is_instance_of(const T_child* child)
{
  return dynamic_cast<const T_parent*>(child) != nullptr;
}

template<typename... T_arg>
inline bool always_return_true(const T_arg&...){return true;}
template<typename... T_arg>
inline bool always_return_false(const T_arg&...){return true;}

class SplashscreenMessage final
{
public:
  SplashscreenMessage(const QString& message);
  ~SplashscreenMessage();

  SplashscreenMessage(const SplashscreenMessage& splashscreenMessage) = delete;
  SplashscreenMessage(SplashscreenMessage&& splashscreenMessage) = delete;
  SplashscreenMessage& operator=(const SplashscreenMessage& splashscreenMessage) = delete;
  SplashscreenMessage& operator=(SplashscreenMessage&& splashscreenMessage) = delete;

  static void show(const QString& message);

private:
  static void push(const QString& message);
  static QString pop();
  static QStack<QString> messageStack;
};

#define SPLASHSCREEN_MESSAGE(message) \
SplashscreenMessage __splashscreenMessage(message); \
Q_UNUSED(__splashscreenMessage);


#define GLRT_EXCEPTION(message) Exception(__FILE__, __LINE__, __FUNCTION__, message)

#define __CALL_SDL(x, action) if(!(x)){qCritical() << #x << " failed: " << SDL_GetError() << "\n("<<__FILE__<<", " << __LINE__ << ")";action}
#define CALL_SDL(x) __CALL_SDL(x,)
#define CALL_SDL_CRITICAL(x) __CALL_SDL(x,abort();)

#define PRINT_VALUE(x) qDebug() << "Value of `" << #x << "` = " << x;

} // namespace glrt


#endif // GLRT_DEPENDENCIES_H
