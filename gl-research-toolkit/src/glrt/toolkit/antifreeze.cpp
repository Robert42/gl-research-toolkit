#include <glrt/toolkit/antifreeze.h>

#define GLRT_USE_ANTIFREEZE 1

/*! \class glrt::Antifreeze

Workaround to prevents some drivers to freeze because of an endless loop within a shader.

*/

namespace glrt {

// ======== Antifreeze =========================================================

Antifreeze::Antifreeze(int numberSecondsToWait)
  : data(new Data)
{
  data->numberSecondsToWait = numberSecondsToWait;

  std::thread thread(std::bind(prevent_freezes, data));
  thread.detach();
}

Antifreeze::~Antifreeze()
{
  data->running = true;
  data = nullptr;
}

void Antifreeze::nextFrame()
{
  data->frame_id++;
}

void Antifreeze::prevent_freezes(Data* data)
{
#if GLRT_USE_ANTIFREEZE
  std::chrono::seconds sleepTime(data->numberSecondsToWait);

  quint32 last_id = data->frame_id;

  while(data->running)
  {
    std::this_thread::sleep_for(sleepTime);

    if(last_id == data->frame_id && !Sleep::isSleeping())
    {
      qCritical() << "Antifreeze detected a freeze!";
      abort();
      break;
    }

    last_id = data->frame_id;
  }
#endif
  delete data;
}

// ======== Sleep ==============================================================

std::atomic<quint32> Antifreeze::Sleep::number_sleeps;

Antifreeze::Sleep::Sleep()
{
  number_sleeps++;
}

Antifreeze::Sleep::~Sleep()
{
  number_sleeps--;
}

bool Antifreeze::Sleep::isSleeping()
{
  return number_sleeps > 0;
}


} // namespace glrt

