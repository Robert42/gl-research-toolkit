#ifndef GLRT_RENDERER_SHADERMANAGER_H
#define GLRT_RENDERER_SHADERMANAGER_H

#include <QObject>

namespace glrt {
namespace renderer {

class ShaderManager : QObject
{
  Q_OBJECT
public:
  ShaderManager();
  ~ShaderManager();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERMANAGER_H
