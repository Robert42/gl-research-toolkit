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

  bool isEmpty() const;

  GLuint load();

  void setFile(const QFileInfo& file);

private:
  QFileInfo _file;

  friend QDebug operator<<(QDebug d, const Texture& t);
};

QDebug operator<<(QDebug d, const Texture& t);

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_CSENE_RESOURCES_TEXTURE_H
