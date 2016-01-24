#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include <QWidget>
#include <QHash>
#include <QTcpSocket>
#include <QBuffer>

namespace Ui {
class ConnectionWidget;
}

class ConnectionWidget : public QWidget
{
  Q_OBJECT

public:
  QString applicationName;

  explicit ConnectionWidget(QTcpSocket* tcpSocket, QWidget *parent = 0);
  ~ConnectionWidget();

signals:
  void applicationNameChanged(const QString& applicationName);

private:
  Ui::ConnectionWidget *ui;
  QTcpSocket* const tcpSocket;
  float frameTime;

  QHash<quintptr, QString> strings;

  void requestString(quintptr ptr);

  void handleData(QBuffer* networkBuffer);
  void updateGui();

private slots:
  void dataReceived();
};

#endif // CONNECTIONWIDGET_H
