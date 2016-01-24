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

  connect(tcpSocket, SIGNAL(aboutToClose()), this, SLOT(deleteLater()));
  connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
  connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));

  requestString(0);
  tcpSocket->flush();
}

ConnectionWidget::~ConnectionWidget()
{
  tcpSocket->abort();

  delete ui;
  delete tcpSocket;
}

void ConnectionWidget::dataReceived()
{
  bool hasData = false;
#if 1 // Use the newsest frame
  qint64 bytes = tcpSocket->bytesAvailable();
  do
  {
    QBuffer networkBuffer;
    networkBuffer.open(QIODevice::ReadOnly);
    if(glrt::Network::readAtomic(&networkBuffer.buffer(), *tcpSocket, 0, 0))
    {
      bytes -= networkBuffer.size();
      hasData = true;
      handleData(&networkBuffer);
    }else
    {
      break;
    }
  }while(tcpSocket->bytesAvailable() > 0 && bytes > 0 && hasData);

  if(hasData)
    updateGui();
#else // Don't skip any data
  QBuffer networkBuffer;
  networkBuffer.open(QIODevice::ReadOnly);
  hasData = glrt::Network::readAtomic(&networkBuffer.buffer(), *tcpSocket, 0, 0);
  if(hasData)
  {
    handleData(&networkBuffer);
    updateGui();
  }
#endif
}

void ConnectionWidget::handleData(QBuffer* networkBuffer)
{
  int n;
  QDataStream stream(networkBuffer);

  stream >> n;
  for(int i=0; i<n; ++i)
  {
    quintptr ptr;
    stream >> ptr;
    stream >> strings[ptr];

    if(ptr == 0 && applicationName!=strings[ptr])
    {
      applicationName = strings[ptr];
      applicationNameChanged(applicationName);
    }
  }

  stream >> n;
  for(int i=0; i<n; ++i)
  {
    quint64 cpuTime;
    quint64 gpuTime;
    quintptr file;
    quintptr function;
    quintptr name;
    int line;
    int depth;
    stream >> cpuTime >> gpuTime >> file >> function >> name >> line >> depth;

    requestString(file);
    requestString(function);
    requestString(name);
  }
  tcpSocket->flush();

  stream >> this->frameTime;
}

void ConnectionWidget::updateGui()
{
  ui->labelFPS->setText(QString("FPS: %0").arg(1.f/this->frameTime));
  qDebug() << frameTime;
}

void ConnectionWidget::requestString(quintptr ptr)
{
  if(!strings.contains(ptr))
    tcpSocket->write(reinterpret_cast<char*>(&ptr), sizeof(quintptr));
}
