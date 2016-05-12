#ifndef GLRT_TCPMESSAGES_H
#define GLRT_TCPMESSAGES_H

#include <glrt/dependencies.h>

#include <QTcpSocket>

namespace glrt {

class TcpMessages
{
public:
  enum class Id : quint32
  {
    Invalid = 0xffffffff,
  };

  struct Message
  {
    Id id;
    QByteArray byteArray;
  };

  QTcpSocket* connection = nullptr;

  TcpMessages();

  void sendMessage(const Message& message);
  bool waitForReadyRead(int msecs = 30000);
  bool messageAvialable();
  Message readMessage();

private:
  struct Header
  {
    Id id = Id::Invalid;
    int bodySize;
  };
};

} // namespace glrt

#endif // GLRT_TCPMESSAGES_H
