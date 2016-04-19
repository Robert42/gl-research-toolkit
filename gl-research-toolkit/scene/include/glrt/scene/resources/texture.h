#ifndef GLRT_CSENE_RESOURCES_TEXTURE_H
#define GLRT_CSENE_RESOURCES_TEXTURE_H

#include <QString>
#include <QFileInfo>
#include <QDebug>

#include <GL/glew.h>

namespace glrt {
namespace scene {
namespace resources {

class Texture
{
public:
  Texture();

  QFileInfo file;

  GLuint load();
};

QDebug operator<<(QDebug d, const Texture& t);

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_CSENE_RESOURCES_TEXTURE_H
