#include <glrt/profiler.h>
#include <glrt/gui/toolbar.h>

#include <SDL2/SDL_timer.h>


namespace glrt {


// ======== Timer ==============================================================


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
  return elapsedTimeAsMicroseconds() * 0.000001f;
}

quint64 Timer::elapsedTimeAsMicroseconds() const
{
  return (quint64(1000000L)*(SDL_GetPerformanceCounter()-last_tick)) / SDL_GetPerformanceFrequency();
}


// ======== Profiler ===========================================================


Profiler* Profiler::activeProfiler = nullptr;


Profiler::Profiler()
{
}


Profiler::~Profiler()
{
}


float Profiler::update()
{
#ifdef GLRT_PROFILER
  recordedScopes.clear();
  recordedScopes.reserve(2048);

  if(printFramerate)
    qDebug() << 1.f / timer.elapsedTimeAsSeconds();
#endif

  return timer.restart();
}


void Profiler::activate()
{
  activeProfiler = this;
  recordedScopes.clear();
}


void Profiler::deactivate()
{
  if(activeProfiler == this)
    activeProfiler = nullptr;
}


// ======== Profiler::Scope ====================================================

const quint64 MAX_TIME = std::numeric_limits<quint64>::max();
const size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

Profiler::Scope::Scope(const char *file, int line, const char *function, const char *name)
  : index(INVALID_INDEX)
{
  if(!activeProfiler)
    return;

  int depth = activeProfiler->currentDepth++;

  index = activeProfiler->recordedScopes.size();
  activeProfiler->recordedScopes.push_back(RecordedScope{MAX_TIME, file, function, name, line, depth});

  this->timer.restart();
}

Profiler::Scope::~Scope()
{
  quint64 elapsedTime = this->timer.elapsedTimeAsMicroseconds();

  if(!activeProfiler)
    return;

   // It's theoretically possible, that a profiler was created during this scope
  bool validIndex = activeProfiler->recordedScopes.size() > index;
  Q_ASSERT(validIndex);

  if(!validIndex)
    return;

  activeProfiler->currentDepth--;
  activeProfiler->recordedScopes[this->index].time = elapsedTime;
}

} // namespace glrt

