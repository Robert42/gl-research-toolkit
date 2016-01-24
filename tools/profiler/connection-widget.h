#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include <QWidget>
#include <QHash>
#include <QTcpSocket>
#include <QBuffer>
#include <QAbstractItemModel>

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
  enum Column
  {
    COLUMN_NAME,
    COLUMN_CPU_TIME,
    COLUMN_GPU_TIME,
    COLUMN_FUNCTION,
    COLUMN_LINE,
    COLUMN_FILE,
    N_COLUMNS
  };

  struct DataLine
  {
    quint64 cpuTime;
    quint64 gpuTime;
    quintptr file;
    quintptr function;
    quintptr name;
    int line;
    int depth;
  };

  class DataModel;

  Ui::ConnectionWidget *ui;
  QVector<DataLine> currentData;
  QTcpSocket* const tcpSocket;
  float frameTime;

  QHash<quintptr, QString> strings;

  void requestString(quintptr ptr);

  void handleData(QBuffer* networkBuffer);
  void updateGui();


  DataModel* model = nullptr;
  QAbstractItemModel* createModel();
  void sendModelStartChangeSignal();
  void sendModelChangedEndSignal();

private slots:
  void dataReceived();
};

#endif // CONNECTIONWIDGET_H
