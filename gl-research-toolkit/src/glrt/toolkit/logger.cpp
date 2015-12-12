#include <glrt/toolkit/logger.h>

#include <QColor>
#include <QCoreApplication>

namespace glrt {

Logger* Logger::logger = nullptr;

inline QString getLogFilePath()
{
  QDir dir(qApp->applicationDirPath());

  QFileInfo executableFile(qApp->applicationFilePath());

  std::cout << dir.absoluteFilePath(executableFile.baseName()+".log.html").toStdString() << std::endl;

  return dir.absoluteFilePath(executableFile.baseName()+".log.html");
}

inline QFile* getLogFile()
{
  static QFile file(getLogFilePath());

  if(!file.open(QFile::WriteOnly))
    Q_ASSERT("Failed to open the logfile for writing");

  return &file;
}

Logger::Logger()
  : logStream(getLogFile())
{

  Q_ASSERT(logger == nullptr);
  logger = this;

  qInstallMessageHandler(messageHandler);
}


Logger::~Logger()
{
  logger = nullptr;
  qInstallMessageHandler(0);
}


void Logger::messageHandler(QtMsgType msgType,
                            const QMessageLogContext& context,
                            const QString& message)
{
  QColor color;
  QColor backColor = QColor::fromRgba(0);
  std::ostream* std_stream = nullptr;

  switch(msgType)
  {
  case QtDebugMsg:
    std_stream = &std::cout;
    color = QColor::fromRgb(0x888a85);
    break;
  case QtWarningMsg:
    std_stream = &std::cerr;
    color = QColor::fromRgb(0xfcaf3e);
    break;
  case QtCriticalMsg:
    std_stream = &std::cerr;
    color = QColor::fromRgb(0xef2929);
    break;
  case QtFatalMsg:
    std_stream = &std::cerr;
    color = QColor::fromRgb(0xeeeeec);
    backColor = QColor::fromRgb(0xa40000);
    break;
  default:
    Q_UNREACHABLE();
  }

  if(std_stream != nullptr)
    *std_stream << message.toStdString() << std::endl;

  logger->logStream << message << "<br>\n";

  if(!logger->handler.isEmpty())
    logger->handler.top()(msgType, context, message);
}


} // namespace glrt

