#ifndef GLRT_CORE_PROFILER_H
#define GLRT_CORE_PROFILER_H

#include <glrt/dependencies.h>

namespace glrt {

class Timer final
{
public:
  quint64 last_tick;

  Timer();

  float restart();
  float elapsedTimeAsSeconds() const;
  quint64 elapsedTimeAsMicroseconds() const;
};

class Profiler final
{
public:
  class Scope;

  Timer timer;
  float frameDuration;
  bool printFramerate = false;

  Profiler();
  ~Profiler();

  float update();
  void activate();
  void deactivate();
private:
  struct RecordedScope
  {
    quint64 time;
    const char* file;
    const char* function;
    const char* name;
    int line;
    int depth;

    void write(QDataStream& stream) const;
  };

  static Profiler* activeProfiler;

  std::vector<RecordedScope> recordedScopes;
  int currentDepth;

  QVector<const char*> strings_to_send;

  void send_data_through_tcp(int port) const;
  void send_data(QDataStream& stream) const;
};

class Profiler::Scope final
{
public:
  Timer timer;
  size_t index;

  Scope(const char* file, int line, const char* function, const char* name);
  ~Scope();
};

} // namespace glrt

#ifdef GLRT_PROFILER
#define PROFILE_SCOPE(name) static Profiler::Scope __profiler_scope_##name(__FILE__, __LINE__, __PRETTY_FUNCTION__, #name);Q_UNUSED(__profiler_scope_##name);
#else
#define PROFILE_SCOPE(name)
#endif

#endif // GLRT_CORE_PROFILER_H
