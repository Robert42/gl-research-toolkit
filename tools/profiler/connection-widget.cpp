#include "connection-widget.h"
#include "ui_connection-widget.h"

ConnectionWidget::ConnectionWidget(QTcpSocket* tcpSocket, QWidget *parent) :
  QWidget(parent),
  applicationName("Connected Game"),
  ui(new Ui::ConnectionWidget),
  tcpSocket(tcpSocket)
{
  ui->setupUi(this);

  requestString(0);

  connect(tcpSocket, SIGNAL(aboutToClose()), this, SLOT(deleteLater()));
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
  QDataStream stream(tcpSocket);

  int n;
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
}

void ConnectionWidget::requestString(quintptr ptr)
{
  if(!strings.contains(ptr))
  {
    QDataStream stream(tcpSocket);
    stream << ptr;
  }
}
