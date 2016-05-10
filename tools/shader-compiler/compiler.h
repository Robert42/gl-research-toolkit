#ifndef COMPILER_H
#define COMPILER_H

#include <QObject>
#include <QStringList>
#include <QElapsedTimer>


class Compiler final : public QObject
{
  Q_OBJECT
public:
  Compiler();
  ~Compiler();

protected:
  void timerEvent(QTimerEvent* event) override;

private:
  QElapsedTimer stillAlive;
  int timerId = 0;
  QStringList receivedCommands;
  QString currentCommand;

  void compile();
};


#endif // COMPILER_H
