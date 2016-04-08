#ifndef GLRT_CSENE_RESOURCES_TEXTURE_H
#define GLRT_CSENE_RESOURCES_TEXTURE_H

#include <QString>
#include <QFileInfo>
#include <QDebug>

namespace glrt {
namespace scene {
namespace resources {

class Texture
{
public:
  Texture();

  QFileInfo file;
};

QDebug operator<<(QDebug d, const Texture& t);

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_CSENE_RESOURCES_TEXTURE_H
