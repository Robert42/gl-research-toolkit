#include <glrt/dependencies.h>
#include <glrt/toolkit/logger.h>
#include <QColor>


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

QDebug operator<<(QDebug d, const glm::uvec2& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::uvec3& v)
{
  return d << glm::to_string(v).c_str();
}

QDebug operator<<(QDebug d, const glm::uvec4& v)
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

QDebug operator<<(QDebug d, const glm::quat& q)
{
  return d << "quat(axis: " << glm::axis(q) << ", angle: " << glm::degrees(glm::angle(q)) << ")";
}

QDebug operator<<(QDebug d, const std::string& s)
{
  return d << QString::fromStdString(s);
}


namespace glrt {

Exception::Exception(const char* file, int line, const char* function, const QString& message)
{
  qCritical() << function << ": " << message << "\n(in " << file << " line:" << line << ")";
  abort();
}

AngelScript::asIScriptEngine* angelScriptEngine = nullptr;


SplashscreenMessage::MessageHandler& SplashscreenMessage::getSplashscreenMessageHandler()
{
  static MessageHandler messageHandler;
  return messageHandler;
}


QStack<QString> SplashscreenMessage::messageStack;

SplashscreenMessage::SplashscreenMessage(const QString& message)
{
  show(message);
  push(message);
}

SplashscreenMessage::~SplashscreenMessage()
{
  show(pop());
}

void SplashscreenMessage::push(const QString& message)
{
  messageStack.push(message);
}

QString SplashscreenMessage::pop()
{
  if(messageStack.isEmpty())
    return QString();

  return messageStack.pop();
}

void SplashscreenMessage::show(const QString& message)
{
  MessageHandler& messageHandler = getSplashscreenMessageHandler();

  if(messageHandler)
    messageHandler(message);

  Logger::SuppressDebug suppressLog;
  qDebug() << "Show SplashScreen message: " << message;
  Q_UNUSED(suppressLog);
}

quint64 magicNumberForString(const char* text)
{
  Q_ASSERT(strlen(text) == 8);

  struct Str
  {
    char str[8];
  };
  union
  {
    quint64 magicNumber;
    Str str;
  };

  std::memcpy(str.str, text, 8);

  return magicNumber;
}


glm::vec3 vec3FromColor(const QColor& c)
{
  return glm::vec3(c.redF(),
                   c.greenF(),
                   c.blueF());
}

glm::vec4 vec4FromColor(const QColor& c)
{
  return glm::vec4(c.redF(),
                   c.greenF(),
                   c.blueF(),
                   c.alphaF());
}

glm::vec3 vec3FromRgb(quint32 rgb)
{
  return vec3FromColor(QColor::fromRgb(rgb));
}

glm::vec4 vec4FromRgba(quint32 rgba)
{
  return vec4FromColor(QColor::fromRgba(rgba));
}


} // namespace glrt
