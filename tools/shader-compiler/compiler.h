#ifndef COMPILER_H
#define COMPILER_H

#include <QObject>
#include <QStringList>
#include <QTcpSocket>


class Compiler final : public QObject
{
  Q_OBJECT
public:
  Compiler();
  ~Compiler();

protected:
  void timerEvent(QTimerEvent* event) override;

private:
  QTcpSocket tcpSocket;

private slots:
  void compile();
  void disconnected();
};


#endif // COMPILER_H
