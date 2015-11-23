#include <glrt/profiler.h>

namespace glrt {

Profiler::Profiler()
{

}

float Profiler::update()
{
  return clock.restart().asSeconds();
}

} // namespace glrt

