#ifndef GLRT_LOGGER_H
#define GLRT_LOGGER_H

#include <glrt/dependencies.h>

namespace glrt {

class Logger final
{
public:
  typedef std::function<bool(QtMsgType, const QMessageLogContext&, const QString&)> MessageHandler;

  QStack<MessageHandler> handler;

  Logger();
  ~Logger();

  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger& operator=(Logger&&) = delete;

private:
  QTextStream logStream;

  static Logger* logger;

  static void messageHandler(QtMsgType msgType,
                             const QMessageLogContext& context,
                             const QString& message);
};

} // namespace glrt

#endif // GLRT_LOGGER_H
