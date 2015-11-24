#include <glrt/profiler.h>

#include <SDL2/SDL_timer.h>

namespace glrt {


Timer::Timer()
{
  last_tick = SDL_GetPerformanceCounter();
}

float Timer::restart()
{

  float elapsedTimeBefore =  elapsedTimeAsSeconds();

  last_tick = SDL_GetPerformanceCounter();

  return elapsedTimeBefore;
}

float Timer::elapsedTimeAsSeconds() const
{
  return elapsedTimeAsMicoseconds() * 0.000001f;
}

quint64 Timer::elapsedTimeAsMicoseconds() const
{
  return (quint64(1000000L)*(SDL_GetPerformanceCounter()-last_tick)) / SDL_GetPerformanceFrequency();
}


Profiler::Profiler()
{

}

float Profiler::update()
{
  return timer.restart();
}

} // namespace glrt

