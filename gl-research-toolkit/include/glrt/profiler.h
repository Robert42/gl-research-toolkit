#ifndef GLRT_PROFILER_H
#define GLRT_PROFILER_H

#include "dependencies.h"

namespace glrt {

class Timer
{
public:
  quint64 last_tick;

  Timer();

  float restart();
  float elapsedTimeAsSeconds() const;
  quint64 elapsedTimeAsMicoseconds() const;
};

class Profiler
{
public:
  Timer timer;

  Profiler();

  float update();
};

} // namespace glrt

#endif // GLRT_PROFILER_H
