#ifndef GLRT_SCENE_RESOURCES_STATICMESHFILE_H
#define GLRT_SCENE_RESOURCES_STATICMESHFILE_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/uuid.h>
#include <glrt/scene/resources/static-mesh.h>

namespace glrt {
namespace scene {
namespace resources {

class StaticMeshFile
{
public:
  StaticMesh staticMesh;

  StaticMeshFile();

  void save(const QFileInfo& filename);
  void load(const QFileInfo& filename);

private:
  struct Header
  {
    quint64 magicNumber = StaticMeshFile::magicNumber();
    quint16 headerLength = sizeof(Header);
    quint16 vertexSize = sizeof(StaticMesh::Vertex);
    quint16 indexedMeshSize = sizeof(StaticMesh::index_type);
    quint16 numVertices = 0;
    quint32 numIndices = 0;
    padding<quint32,3> _padding;
  };

  static quint64 magicNumber();
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_STATICMESHFILE_H
