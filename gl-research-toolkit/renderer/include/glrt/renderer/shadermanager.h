#ifndef GLRT_RENDERER_SHADERMANAGER_H
#define GLRT_RENDERER_SHADERMANAGER_H

#include "managed-shader.h"

namespace glrt {
namespace renderer {

class ShaderManager : QObject
{
  Q_OBJECT
public:
  QList<QDir> shaderIncludeDirs; // directories used by #include from within the shaders
  QList<QDir> shaderSourceDirs; // directories, where to look for sahders with the extensions shaderExtensions
  QStringList shaderExtensions = {".cs", ".fs", ".vs"};

  ShaderManager();
  ~ShaderManager();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERMANAGER_H
