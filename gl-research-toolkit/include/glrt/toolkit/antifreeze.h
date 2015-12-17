#ifndef GLRT_ANTIFREEZE_H
#define GLRT_ANTIFREEZE_H

#include <glrt/dependencies.h>

namespace glrt {

class Antifreeze
{
public:
  Antifreeze();
  ~Antifreeze();

  Antifreeze(Antifreeze&&) = delete;
  Antifreeze(const Antifreeze&) = delete;
  Antifreeze&operator=(Antifreeze&&) = delete;
  Antifreeze&operator=(const Antifreeze&) = delete;
};

} // namespace glrt

#endif // GLRT_ANTIFREEZE_H
