#ifndef GLRT_TEMPSHADERFILE_H
#define GLRT_TEMPSHADERFILE_H

#include "dependencies.h"

namespace glrt {

class TempShaderFile final
{
public:
  TempShaderFile();
  ~TempShaderFile();

  static void init();

private:
  static void clearTempDir();
  static QDir tempDir();
};

} // namespace glrt

#endif // GLRT_TEMPSHADERFILE_H
