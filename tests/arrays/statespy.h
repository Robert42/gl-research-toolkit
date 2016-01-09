#ifndef STATESPY_H
#define STATESPY_H

#include <QString>
#include <QHash>

class StateSpy
{
public:
  typedef QHash<int, QString> State;

  int index;

  StateSpy();
  StateSpy(const StateSpy& s);
  StateSpy(StateSpy&& s);

  ~StateSpy();

  QString& state();

  static State& globalState();
};

#endif // STATESPY_H
