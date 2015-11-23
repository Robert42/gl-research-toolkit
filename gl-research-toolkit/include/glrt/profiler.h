#ifndef GLRT_PROFILER_H
#define GLRT_PROFILER_H

#include "dependencies.h"

namespace glrt {

class Profiler
{
public:
  sf::Clock clock;

  Profiler();

  float update();
};

} // namespace glrt

#endif // GLRT_PROFILER_H
