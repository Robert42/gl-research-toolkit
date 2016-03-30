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
  struct ArrayMesh
  {
    Uuid<StaticMesh> uuid;
    quint32 vertexDataStart;
    quint32 vertexDataLength;
    quint32 numVertices;
    quint32 _padding;
  };
  struct IndexedMesh : public ArrayMesh
  {
    quint32 indexDataStart;
    quint32 indexDataLength;
    quint32 numIndices;
    quint32 _padding;
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
  };

  StaticMeshFile();

  static quint64 magicNumber();
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_STATICMESHFILE_H
