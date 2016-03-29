#ifndef GLRT_CSENE_RESOURCES_TEXTURE_H
#define GLRT_CSENE_RESOURCES_TEXTURE_H

#include <QString>
#include <QFileInfo>

namespace glrt {
namespace scene {
namespace resources {

class Texture
{
public:
  Texture();

  QFileInfo file;
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_CSENE_RESOURCES_TEXTURE_H
