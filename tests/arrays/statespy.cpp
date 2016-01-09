#include "statespy.h"
#include <QDebug>

bool StateSpy::enabledDebugPrint = false;

StateSpy::StateSpy()
  : index(nextIndex())
{
  print("default constructor");
}

StateSpy::StateSpy(const StateSpy& s)
  : index(nextIndex())
{
  print(QString("copy constructor from %0").arg(s.index));
}

StateSpy::StateSpy(StateSpy&& s)
  : index(nextIndex())
{
  print(QString("move constructor from %0").arg(s.index));
}

StateSpy::~StateSpy()
{
  print(QString("destructed"));
}


QString& StateSpy::state()
{
  return globalState()[index];
}

void StateSpy::print(const QString& s)
{
  globalState()[-1] += QString("%0: %1\n").arg(index).arg(s);
  state() += s + '\n';
  if(enabledDebugPrint)
    qDebug() << QString("%0: %1").arg(index).arg(s);
}

void StateSpy::clear()
{
  globalState().clear();
}

QString StateSpy::log()
{
  if(globalState().contains(-1))
    return globalState()[-1];
  return "";
}

StateSpy::State& StateSpy::globalState()
{
  static State s;
  return s;
}

int StateSpy::nextIndex()
{
  return globalState().isEmpty() ? 0 : globalState().keys().last()+1;
}
