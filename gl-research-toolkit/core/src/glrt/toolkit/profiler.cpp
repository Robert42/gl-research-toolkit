#include <glrt/toolkit/profiler.h>
#include <glrt/toolkit/network.h>

#include <SDL2/SDL_timer.h>

#include <QDataStream>
#include <QHostAddress>


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


Profiler::Profiler(const QString& applicationName)
  : applicationName(applicationName)
{
  connect(&tcpSocket, &QTcpSocket::disconnected, this, &Profiler::deactivate);
  connect(&tcpSocket, &QTcpSocket::readyRead, this, &Profiler::readStringsToWrite);
  connect(&tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError()));
}


Profiler::~Profiler()
{
}


float Profiler::update()
{
  float elapsedTime = timer.restart();

#ifdef GLRT_PROFILER
  recordedScopes.clear();
  if(this->isActive())
  {
    recordedScopes.reserve(2048);

    if(printFramerate)
      qDebug() << 1.f / timer.elapsedTimeAsSeconds();

    send_data_through_tcp(elapsedTime);
  }
#endif

  return elapsedTime;
}


bool Profiler::isActive() const
{
  return activeProfiler == this;
}


void Profiler::activate()
{
  activeProfiler = this;
  recordedScopes.clear();
  tcpSocket.connectToHost(QHostAddress::LocalHost, GLRT_PROFILER_DEFAULT_PORT);
  if(!tcpSocket.waitForConnected(1000))
    deactivate();
}


void Profiler::deactivate()
{
  if(activeProfiler == this)
  {
    activeProfiler = nullptr;
    tcpSocket.abort();
  }
}

void Profiler::send_data_through_tcp(float frameTime)
{
  if(tcpSocket.isOpen())
  {
    QBuffer networkBuffer;
    networkBuffer.open(QIODevice::WriteOnly);
    QDataStream stream(&networkBuffer);

    send_data(stream);
    stream << frameTime;

    Network::writeAtomic(&tcpSocket, networkBuffer.buffer());
  }
}

void Profiler::send_data(QDataStream& stream)
{
  stream << int(strings_to_send.size());
  for(quintptr ptr : strings_to_send.keys())
    stream << ptr << strings_to_send[ptr];
  strings_to_send.clear();

  stream << int(recordedScopes.size());
  for(const RecordedScope& s : recordedScopes)
    s.write(stream);
}

void Profiler::connectionError()
{
  qWarning() << tcpSocket.errorString();
  deactivate();
}

void Profiler::readStringsToWrite()
{
  if(tcpSocket.bytesAvailable() < int(sizeof(quintptr)))
    return;

  QDataStream stream(&tcpSocket);

  while(!stream.atEnd())
  {
    quintptr ptr;
    stream >> ptr;
    if(ptr == 0)
      strings_to_send[0] = applicationName;
    else
      strings_to_send[ptr] = reinterpret_cast<char*>(ptr);

  }
}


// ======== Profiler::RecordedScope ============================================

void Profiler::RecordedScope::write(QDataStream& stream) const
{
  stream << cpuTime << gpuTime << quintptr(file) << quintptr(function) << quintptr(name) << line << depth;
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
  activeProfiler->recordedScopes.push_back(RecordedScope{MAX_TIME, MAX_TIME, file, function, name, line, depth});

  this->timer.restart();
}

Profiler::Scope::~Scope()
{
  quint64 elapsedCpuTime = this->timer.elapsedTimeAsMicroseconds();

  if(!activeProfiler)
    return;

   // It's theoretically possible, that a profiler was created during this scope
  bool validIndex = activeProfiler->recordedScopes.size() > index;
  Q_ASSERT(validIndex);

  if(!validIndex)
    return;

  activeProfiler->currentDepth--;
  activeProfiler->recordedScopes[this->index].cpuTime = elapsedCpuTime;
  activeProfiler->recordedScopes[this->index].gpuTime = MAX_TIME; // #TODO:::: https://www.opengl.org/wiki/Query_Object#Query_scope
}

} // namespace glrt

