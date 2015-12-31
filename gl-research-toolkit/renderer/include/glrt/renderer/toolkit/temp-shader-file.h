#ifndef GLRT_RENDERER_TEMPSHADERFILE_H
#define GLRT_RENDERER_TEMPSHADERFILE_H

#include <glrt/dependencies.h>

namespace glrt {
namespace renderer {

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

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_TEMPSHADERFILE_H
