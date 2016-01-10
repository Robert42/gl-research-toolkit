#include "statespy.h"
#include <QDebug>

bool StateSpy::enabledDebugPrint = false;
int StateSpy::_nextIndex = 0;

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

StateSpy& StateSpy::operator=(const StateSpy& s)
{
  print(QString("assignment operator from %0").arg(s.index));
  return *this;
}

StateSpy& StateSpy::operator=(StateSpy&& s)
{
  print(QString("move operator from %0").arg(s.index));
  return *this;
}

void StateSpy::print(const QString& s)
{
  globalState() += QString("%0: %1\n").arg(index).arg(s);
  if(enabledDebugPrint)
    qDebug() << QString("%0: %1").arg(index).arg(s);
}

void StateSpy::clearLog()
{
  globalState().clear();
}

void StateSpy::clearIndex()
{
  _nextIndex = 0;
}

QString StateSpy::log()
{
  return globalState();
}

QString& StateSpy::globalState()
{
  static QString s;
  return s;
}

int StateSpy::nextIndex()
{
  return _nextIndex++;
}
