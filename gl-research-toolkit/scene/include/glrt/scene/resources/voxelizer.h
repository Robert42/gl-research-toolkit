#ifndef GLRT_SCENE_RESOURCES_VOXELIZER_H
#define GLRT_SCENE_RESOURCES_VOXELIZER_H

#include <glrt/toolkit/uuid.h>
#include <glrt/scene/resources/static-mesh.h>
#include <glrt/scene/resources/material.h>
#include <glrt/scene/resources/utilities/gl-texture.h>
#include <glrt/scene/coord-frame.h>

namespace glrt {
namespace scene {
namespace resources {

class ResourceIndex;

class Voxelizer final
{
public:
  class Implementation;

  enum class FieldType : quint32
  {
    SIGNED_DISTANCE_FIELD,
  };

  enum class MeshType : quint32
  {
    FACE_SIDE,
    MANIFOLD_RAY_CHECK,
    TWO_SIDED
  };

  static QString toString(MeshType meshType);
  static QString toAngelScript(MeshType meshType);

  ResourceIndex* resourceIndex = nullptr;
  bool voxelizing_scene = false;

  struct Hints
  {
    utilities::GlTexture::Type voxelType = utilities::GlTexture::Type::FLOAT16;
    float extend = 1.f;
    int minSize = 4;
    int maxSize = 256;
    float scaleFactor = 1.f;
    bool enabled = true;

    Hints()
    {
    }
  };

  Hints signedDistanceField;


  Voxelizer(ResourceIndex* resourceIndex);
  ~Voxelizer();

  static void registerAngelScriptAPI();

  void voxelize(const Uuid<StaticMesh>& staticMeshUuid, MeshType meshType);

  void beginJoinedGroup();
  void addToGroup(const Uuid<StaticMesh>& meshUuid, const CoordFrame& frame, bool two_sided, const Uuid<StaticMesh>& proxyMesh);
  void addToGroup(const Uuid<StaticMesh>& meshUuid, const CoordFrame& frame, bool two_sided);
  void voxelizeJoinedGroup(MeshType meshType);

private:
  struct FileNames
  {
    ResourceIndex* resourceIndex;
    Uuid<StaticMesh> staticMeshUuid;
    QString staticMeshFileName;
    QString voxelFileName;
    bool shouldRevoxelizeMesh;

    FileNames(ResourceIndex* resourceIndex, const Uuid<StaticMesh>& staticMeshUuid);
    FileNames(ResourceIndex* resourceIndex, const QSet<Uuid<StaticMesh> >& staticMeshUuids, const Uuid<StaticMesh>& instanceAnchor);
  };

  void revoxelizeMesh(const FileNames& filenames, MeshType meshType, Hints signedDistanceField = Hints());
  void revoxelizeMesh(const TriangleArray& vertices, size_t rawMeshDataSize, const FileNames& filenames, MeshType meshType, Hints signedDistanceField = Hints());
  void registerToIndex(const FileNames& filenames);

  static QString voxelMetaDataFilenameForMesh(const QString& staticMeshFileName);
};

uint qHash(glrt::scene::resources::Voxelizer::FieldType type);


class Voxelizer::Implementation : public QObject
{
public:
  typedef Voxelizer::MeshType MeshType;

  static Voxelizer::Implementation* singleton;

  Implementation();
  ~Implementation();

  virtual utilities::GlTexture distanceField(const glm::ivec3& gridSize, const CoordFrame& localToVoxelSpace, const TriangleArray& staticMesh, MeshType meshType) = 0;
};


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_VOXELIZER_H
