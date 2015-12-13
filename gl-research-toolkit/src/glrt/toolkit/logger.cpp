#include <glrt/toolkit/logger.h>

#include <QCoreApplication>

namespace glrt {

Logger* Logger::logger = nullptr;
QStack<Logger::MessageHandler> Logger::handler;

inline QString getLogFilePath()
{
  QDir dir(qApp->applicationDirPath());

  QFileInfo executableFile(qApp->applicationFilePath());

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
  logStream.setAutoFormatting(true);
  logStream.setAutoFormattingIndent(2);
  logStream.writeStartElement("body");
  logStream.writeAttribute("bgcolor", "#2e3436");

  Q_ASSERT(logger == nullptr);
  logger = this;

  qInstallMessageHandler(messageHandler);
}


Logger::~Logger()
{
  logStream.writeEndElement();

  logger = nullptr;
  qInstallMessageHandler(0);
}


void Logger::messageHandler(QtMsgType msgType,
                            const QMessageLogContext& context,
                            const QString& message)
{
  QString color;
  QString backColor;
  std::ostream* std_stream = nullptr;

  switch(msgType)
  {
  case QtDebugMsg:
    std_stream = &std::cout;
    color = QString("#babdb6");
    break;
  case QtWarningMsg:
    std_stream = &std::cerr;
    color = QString("#fcaf3e");
    break;
  case QtCriticalMsg:
    std_stream = &std::cerr;
    color = QString("#ef2929");
    break;
  case QtFatalMsg:
    std_stream = &std::cerr;
    color = QString("#eeeeec");
    backColor = QString("#a40000");
    break;
  default:
    Q_UNREACHABLE();
  }

  if(!message.trimmed().isEmpty())
  {
  logger->logStream.writeStartElement("font");
  logger->logStream.writeAttribute("color", color);
  if(!backColor.isEmpty())
    logger->logStream.writeAttribute("style", QString("background-color: %0;").arg(backColor));
  logger->logStream.writeCharacters(message);
  logger->logStream.writeEndElement();
  logger->logStream.writeEmptyElement("br");

    logger->logStream.writeStartElement("font");
    logger->logStream.writeAttribute("color", "#555753");
    logger->logStream.writeCharacters(QString("(Function %3, file %1, line %2)").arg(context.file).arg(context.line).arg(context.function));
    logger->logStream.writeEndElement();
    logger->logStream.writeEmptyElement("br");
    logger->logStream.writeEmptyElement("br");
  }

  bool alreadyHandeled = false;

  if(!logger->handler.isEmpty())
    alreadyHandeled = logger->handler.top()(msgType, context, message);

  if(!alreadyHandeled && std_stream != nullptr)
    *std_stream << message.toStdString() << std::endl;
}

// ======== Logger::Suppress ===================================================

Logger::SuppressDebug::SuppressDebug()
{
  Logger::handler.push(std::bind(&SuppressDebug::handleMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  stack_size = Logger::handler.size();
}

Logger::SuppressDebug::~SuppressDebug()
{
  Q_ASSERT(stack_size==Logger::handler.size());
  Logger::handler.pop();
}

bool Logger::SuppressDebug::handleMessage(QtMsgType type, const QMessageLogContext&, const QString&)
{
  // suppress debug messages
  return type==QtDebugMsg;
}


} // namespace glrt

