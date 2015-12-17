#ifndef GLRT_ANTIFREEZE_H
#define GLRT_ANTIFREEZE_H

#include <glrt/dependencies.h>

namespace glrt {

class Antifreeze
{
public:
  class Sleep;

  explicit Antifreeze(int numberSecondsToWait=5);
  ~Antifreeze();

  Antifreeze(Antifreeze&&) = delete;
  Antifreeze(const Antifreeze&) = delete;
  Antifreeze& operator=(Antifreeze&&) = delete;
  Antifreeze& operator=(const Antifreeze&) = delete;

  void nextFrame();

private:
  struct Data
  {
    std::atomic<quint32> frame_id;
    int numberSecondsToWait = 5;
    bool running;
  };

  Data* data;

  static void prevent_freezes(Data* data);
};


class Antifreeze::Sleep
{
public:
  Sleep();
  ~Sleep();

  Sleep(Sleep&&) = delete;
  Sleep(const Sleep&) = delete;
  Sleep& operator=(Sleep&&) = delete;
  Sleep& operator=(const Sleep&) = delete;

  static bool isSleeping();

private:
  static std::atomic<quint32> number_sleeps;
};


} // namespace glrt

#endif // GLRT_ANTIFREEZE_H
