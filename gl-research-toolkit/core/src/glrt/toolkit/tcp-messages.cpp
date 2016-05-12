#include <glrt/toolkit/tcp-messages.h>

#include <QElapsedTimer>

namespace glrt {

TcpMessages::TcpMessages()
{

}

void TcpMessages::sendMessage(const Message& message)
{
  Q_ASSERT(connection != nullptr);

  Header header;
  header.id = message.id;
  header.bodySize = message.byteArray.length();

  connection->write(reinterpret_cast<char*>(&header), qint64(sizeof(header)));
  connection->write(message.byteArray);
}

bool TcpMessages::waitForReadyRead(int msecs)
{
  Q_ASSERT(connection != nullptr);

  QElapsedTimer timer;
  timer.start();

  while(timer.elapsed() >= msecs)
  {
    if(messageAvialable())
      return true;

    qint64 waitTime = qint64(msecs) - timer.elapsed();
    waitTime = glm::clamp<qint64>(waitTime, 0, std::numeric_limits<int>::max());

    if(!connection->waitForReadyRead(int(waitTime)))
      return false;
  }

  return false;
}

bool TcpMessages::messageAvialable()
{
  Q_ASSERT(connection != nullptr);

  const qint64 headerSize = qint64(sizeof(Header));

  qint64 bytesAvialable = connection->bytesAvailable();

  if(bytesAvialable < headerSize)
    return false;

  Header header;

  connection->peek(reinterpret_cast<char*>(&header), headerSize);

  return header.bodySize+headerSize <= bytesAvialable;
}

TcpMessages::Message TcpMessages::readMessage()
{
  Q_ASSERT(connection != nullptr);

  const qint64 headerSize = qint64(sizeof(Header));
  Header header;
  TcpMessages::Message message;

  if(!messageAvialable())
    return message;

  connection->read(reinterpret_cast<char*>(&header), headerSize);
  message.byteArray = connection->read(header.bodySize);
  message.id = header.id;

  return message;
}


} // namespace glrt
