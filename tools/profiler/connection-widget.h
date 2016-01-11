#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class ConnectionWidget;
}

class ConnectionWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ConnectionWidget(QTcpSocket* tcpSocket, QWidget *parent = 0);
  ~ConnectionWidget();

private:
  Ui::ConnectionWidget *ui;
  QTcpSocket* const tcpSocket;
};

#endif // CONNECTIONWIDGET_H
