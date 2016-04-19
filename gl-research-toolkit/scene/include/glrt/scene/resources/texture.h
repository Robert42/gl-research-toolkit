#ifndef GLRT_CSENE_RESOURCES_TEXTURE_H
#define GLRT_CSENE_RESOURCES_TEXTURE_H

#include <QString>
#include <QFileInfo>
#include <QDebug>
#include <QSharedPointer>

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

  void clear();
  void setFile(const QFileInfo& file);

private:
  class Source;
  class Empty;
  class FileSource;

  QSharedPointer<Source> source;

  friend QDebug operator<<(QDebug d, const Texture& t);
};

QDebug operator<<(QDebug d, const Texture& t);

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_CSENE_RESOURCES_TEXTURE_H
