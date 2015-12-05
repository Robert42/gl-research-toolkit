#ifndef GLRT_TEMPSHADERFILE_H
#define GLRT_TEMPSHADERFILE_H

#include <glrt/dependencies.h>

namespace glrt {

class TempShaderFile final
{
public:
  TempShaderFile();
  ~TempShaderFile();

  void addPreprocessorBlock(const QStringList& preprocessorBlock);

  static void init();

private:
  static void clearTempDir();
  static QDir tempDir();

  static int instanceCounter;
};

} // namespace glrt

#endif // GLRT_TEMPSHADERFILE_H
