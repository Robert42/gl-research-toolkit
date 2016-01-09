#ifndef STATESPY_H
#define STATESPY_H

#include <QString>
#include <QMap>

class StateSpy
{
public:
  typedef QMap<int, QString> State;

  int index;

  StateSpy();
  StateSpy(const StateSpy& s);
  StateSpy(StateSpy&& s);

  ~StateSpy();

  void print(const QString& s);
  QString& state();

  static void clear();
  static QString log();
  static bool enabledDebugPrint;

private:
  static int nextIndex();
  static State& globalState();
};

#endif // STATESPY_H
