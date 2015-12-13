#ifndef GLRT_LOGGER_H
#define GLRT_LOGGER_H

#include <glrt/dependencies.h>

#include <QXmlStreamWriter>

namespace glrt {

class Logger final
{
public:
  class SuppressDebug;

  typedef std::function<bool(QtMsgType, const QMessageLogContext&, const QString&)> MessageHandler;

  static QStack<MessageHandler> handler;

  Logger();
  ~Logger();

  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger& operator=(Logger&&) = delete;

private:
  QXmlStreamWriter logStream;

  static Logger* logger;

  static void messageHandler(QtMsgType msgType,
                             const QMessageLogContext& context,
                             const QString& message);
};


class Logger::SuppressDebug final
{
public:
  SuppressDebug();
  ~SuppressDebug();

  bool handleMessage(QtMsgType, const QMessageLogContext&, const QString&);

  SuppressDebug(const SuppressDebug&) = delete;
  SuppressDebug(SuppressDebug&&) = delete;
  SuppressDebug& operator=(const SuppressDebug&) = delete;
  SuppressDebug& operator=(SuppressDebug&&) = delete;

private:
 int stack_size;
};


} // namespace glrt

#endif // GLRT_LOGGER_H
