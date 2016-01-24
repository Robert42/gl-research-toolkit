#ifndef GLRT_CORE_PROFILER_H
#define GLRT_CORE_PROFILER_H

#include <glrt/dependencies.h>
#include <QTcpSocket>

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

class Profiler final : public QObject
{
  Q_OBJECT
public:
  class Scope;

  Timer timer;
  float frameDuration;
  bool printFramerate = false;
  QString applicationName;

  Profiler(const QString& applicationName);
  Profiler(const Profiler&) = delete;
  ~Profiler();


  float update();
  void activate();
  void deactivate();
  bool isActive() const;
private:
  struct RecordedScope
  {
    quint64 cpuTime;
    quint64 gpuTime;
    const char* file;
    const char* function;
    const char* name;
    int line;
    int depth;

    void write(QDataStream& stream) const;
  };

  static int frameId;
  static Profiler* activeProfiler;

  std::vector<RecordedScope> recordedScopes;
  int currentDepth;

  QHash<quintptr, QString> strings_to_send;
  QTcpSocket tcpSocket;

  void send_data_through_tcp(float frameTime);
  void send_data(QDataStream& stream);

private slots:
  void connectionError();
  void readStringsToWrite();
};

class Profiler::Scope final
{
public:
  Timer timer;
  size_t index;
  int frameId = -1;

  Scope(const char* file, int line, const char* function, const char* name);
  ~Scope();
};

} // namespace glrt

#ifdef GLRT_PROFILER
#define PROFILE_SCOPE(name) glrt::Profiler::Scope __profiler_scope_##__LINE__(__FILE__, __LINE__, __PRETTY_FUNCTION__, name);Q_UNUSED(__profiler_scope_##__LINE__);
#else
#define PROFILE_SCOPE(name)
#endif

#endif // GLRT_CORE_PROFILER_H
