#include "connection-widget.h"
#include "ui_connection-widget.h"

#include <glrt/toolkit/network.h>

ConnectionWidget::ConnectionWidget(QTcpSocket* tcpSocket, QWidget *parent) :
  QWidget(parent),
  applicationName("Connected Game"),
  ui(new Ui::ConnectionWidget),
  tcpSocket(tcpSocket)
{
  ui->setupUi(this);

  requestString(0);
  tcpSocket->flush();

  connect(tcpSocket, SIGNAL(aboutToClose()), this, SLOT(deleteLater()));
  connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
  connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}

ConnectionWidget::~ConnectionWidget()
{
  tcpSocket->abort();

  delete ui;
  delete tcpSocket;
}

void ConnectionWidget::dataReceived()
{
  int n;
  QBuffer networkBuffer;
  networkBuffer.open(QIODevice::ReadOnly);

  bool hasData = false;
#if 1 // Use the newsest frame
  do
  {
    networkBuffer.seek(0);
    networkBuffer.buffer().clear();
    hasData = glrt::Network::readAtomic(&networkBuffer.buffer(), *tcpSocket, 10000);


    QDataStream stream(&networkBuffer);
    stream >> n;
    for(int i=0; i<n; ++i)
    {
      quintptr ptr;
      stream >> ptr >> strings[ptr];

      if(ptr == 0 && applicationName!=strings[ptr])
      {
        applicationName = strings[ptr];
        applicationNameChanged(applicationName);
      }
    }

    if(tcpSocket->bytesAvailable() > 0 && hasData)
      continue;
  }while(false);
#else // Don't skip any data
  hasData = glrt::Network::readAtomic(&networkBuffer.buffer(), *tcpSocket, 10000);
#endif

  if(!hasData)
    return;

  QDataStream stream(&networkBuffer);

  stream >> n;
  for(int i=0; i<n; ++i)
  {
    quintptr ptr;
    stream >> ptr >> strings[ptr];

    if(ptr == 0 && applicationName!=strings[ptr])
    {
      applicationName = strings[ptr];
      applicationNameChanged(applicationName);
    }
  }

  stream >> n;
  for(int i=0; i<n; ++i)
  {
    quint64 time;
    quintptr file;
    quintptr function;
    quintptr name;
    int line;
    int depth;
    stream >> time >> file >> function >> name >> line >> depth;

    requestString(file);
    requestString(function);
    requestString(name);
  }
  tcpSocket->flush();
}

void ConnectionWidget::requestString(quintptr ptr)
{
  if(!strings.contains(ptr))
    tcpSocket->write(reinterpret_cast<char*>(&ptr), sizeof(quintptr));
}
