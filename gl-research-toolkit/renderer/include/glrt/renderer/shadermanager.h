#ifndef GLRT_RENDERER_SHADERMANAGER_H
#define GLRT_RENDERER_SHADERMANAGER_H

#include "managed-shader.h"

namespace glrt {
namespace renderer {

class ShaderManager : QObject
{
  Q_OBJECT
public:
  ShaderManager();
  ~ShaderManager();

private:
  QList<QDir> shaderSourceDirs; // directories, where to look for shaders with the extensions shaderExtensions
  QStringList shaderExtensions = {".cs", ".fs", ".vs"};
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERMANAGER_H