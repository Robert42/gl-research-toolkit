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
  QMap<Uuid<StaticMesh>, StaticMesh> staticMeshes;

  StaticMeshFile();

  void save(const QFileInfo& filename);
  void load(const QFileInfo& filename);

private:
  struct ArrayMesh
  {
    Uuid<StaticMesh> uuid;
    quint32 indexOfFirstVertex;
    quint32 numVertices;
  };
  struct IndexedMesh : public ArrayMesh
  {
    quint32 firstIndex;
    quint32 numIndices;
  };

  struct Header
  {
    quint64 magicNumber = StaticMeshFile::magicNumber();
    quint16 headerLength = sizeof(Header);
    quint16 unindexedMeshSize = sizeof(ArrayMesh);
    quint16 indexedMeshSize = sizeof(IndexedMesh);
    quint16 numArrayMeshes = 0;
    quint16 numIndexedMeshes = 0;
    padding<quint16, 3> _padding;
    quint32 totalNumVertices = 0;
    quint32 totalNumIndices = 0;
    padding<quint32, 2> _reserved;
  };

  static quint64 magicNumber();
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_STATICMESHFILE_H
