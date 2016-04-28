#ifndef GLRT_DEPENDENCIES_H
#define GLRT_DEPENDENCIES_H

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/bit.hpp>
#include <glm/gtc/round.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>

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
#include <QQueue>
#include <QPointer>
#include <QMutexLocker>
#include <QMutex>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <cstring>
#include <type_traits>

#include <SDL2/SDL.h>

typedef quint8 byte;

template<typename T>
inline int qHash(const QPointer<T>& ptr, int seed = 0)
{
  return qHash(ptr.data(), seed);
}


QDebug operator<<(QDebug d, const glm::bvec2& v);
QDebug operator<<(QDebug d, const glm::bvec3& v);
QDebug operator<<(QDebug d, const glm::bvec4& v);

QDebug operator<<(QDebug d, const glm::vec2& v);
QDebug operator<<(QDebug d, const glm::vec3& v);
QDebug operator<<(QDebug d, const glm::vec4& v);

QDebug operator<<(QDebug d, const glm::ivec2& v);
QDebug operator<<(QDebug d, const glm::ivec3& v);
QDebug operator<<(QDebug d, const glm::ivec4& v);

QDebug operator<<(QDebug d, const glm::uvec2& v);
QDebug operator<<(QDebug d, const glm::uvec3& v);
QDebug operator<<(QDebug d, const glm::uvec4& v);

QDebug operator<<(QDebug d, const glm::mat2& v);
QDebug operator<<(QDebug d, const glm::mat3& v);
QDebug operator<<(QDebug d, const glm::mat4& v);

QDebug operator<<(QDebug d, const glm::quat& q);

QDebug operator<<(QDebug d, const std::string& s);


namespace glrt {

template<typename T>
inline T sq(T x)
{
  return x;
}

using AngelScript::asDWORD;

const asDWORD ACCESS_MASK_RESOURCE_LOADING = AngelScriptIntegration::ACCESS_MASK_USER;
const asDWORD ACCESS_MASK_USER = ACCESS_MASK_RESOURCE_LOADING<<1;

glm::vec3 vec3FromColor(const QColor& c);
glm::vec4 vec4FromColor(const QColor& c);
glm::vec3 vec3FromRgb(quint32 rgb);
glm::vec4 vec4FromRgba(quint32 rgba);

class Exception final
{
public:
  Exception(const char* file, int line, const char* function, const QString& message);
};

template<typename T, int N=1>
struct padding final
{
public:
  void clear()
  {
    size_t s = sizeof(T)*N;
    for(size_t i=0; i<s; ++i)
      _[i] = 0xcc;
  }

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
inline bool always_return_false(const T_arg&...){return false;}

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

  typedef std::function<void(const QString&)> MessageHandler;

  static MessageHandler& getSplashscreenMessageHandler();

private:
  static void push(const QString& message);
  static QString pop();
  static QStack<QString> messageStack;
};

#define SPLASHSCREEN_MESSAGE(message) \
::glrt::SplashscreenMessage __splashscreenMessage(message); \
Q_UNUSED(__splashscreenMessage);

quint64 magicNumberForString(const char* text);


#define GLRT_EXCEPTION(message) Exception(__FILE__, __LINE__, __FUNCTION__, message)

#define __CALL_SDL(x, action) if(!(x)){qCritical() << #x << " failed: " << SDL_GetError() << "\n("<<__FILE__<<", " << __LINE__ << ")";action}
#define CALL_SDL(x) __CALL_SDL(x,)
#define CALL_SDL_CRITICAL(x) __CALL_SDL(x,abort();)

#define PRINT_VALUE(x) qDebug() << "Value of `" << #x << "` = " << (x);

} // namespace glrt


#endif // GLRT_DEPENDENCIES_H
