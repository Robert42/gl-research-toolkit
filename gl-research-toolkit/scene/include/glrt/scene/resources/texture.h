#ifndef GLRT_CSENE_RESOURCES_TEXTURE_H
#define GLRT_CSENE_RESOURCES_TEXTURE_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/uuid.h>
#include <glrt/scene/resources/static-mesh.h>

#include <GL/glew.h>

namespace glrt {
namespace scene {
namespace resources {

class ResourceManager;

class Texture
{
public:
  enum class VoxelizationType
  {
    VOXELIZED_GREY,
  };

  Texture();

  bool isEmpty() const;

  GLuint load(ResourceManager& resourceManager);

  void clear();
  void setFile(const QFileInfo& file);
  void setVoxelizedStaticMesh(const Uuid<StaticMesh>& staticMesh, VoxelizationType voxelizationType);

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
