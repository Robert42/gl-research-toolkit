#include "connection-widget.h"
#include "ui_connection-widget.h"

ConnectionWidget::ConnectionWidget(QTcpSocket* tcpSocket, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ConnectionWidget),
  tcpSocket(tcpSocket)
{
  ui->setupUi(this);

  connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
}

ConnectionWidget::~ConnectionWidget()
{
  delete ui;
  delete tcpSocket;
}
