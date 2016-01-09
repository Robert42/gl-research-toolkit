#include "statespy.h"

StateSpy::StateSpy()
  : index(globalState().isEmpty() ? 0 : globalState().keys().last()+1)
{
  state() += "default constructor\n";
}

StateSpy::StateSpy(const StateSpy& s)
  : StateSpy()
{
  state() += QString("copy constructor from %0\n").arg(s.index);
}

StateSpy::StateSpy(StateSpy&& s)
  : StateSpy()
{
  state() += QString("move constructor from %0\n").arg(s.index);
}

StateSpy::~StateSpy()
{
  state() += QString("destructed");
}


QString& StateSpy::state()
{
  return globalState()[index];
}

StateSpy::State& StateSpy::globalState()
{
  static State s;
  return s;
}
