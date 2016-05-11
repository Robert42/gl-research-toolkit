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
    DEFAULT,
    TWO_SIDED
  };

  ResourceIndex* resourceIndex = nullptr;

  struct Hints
  {
    utilities::GlTexture::Type voxelType = utilities::GlTexture::Type::FLOAT16;
    float extend = 1.f;
    int minSize = 4;
    int maxSize = 256; // #FIXME: why doesn't 128 work for sponza?
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

private:
  static void revoxelizeMesh(const Uuid<StaticMesh>& staticMeshUuid, const QString& staticMeshFileName, const QString& voxelFileName, MeshType meshType, Hints signedDistanceField = Hints());
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

  virtual utilities::GlTexture distanceField(const glm::ivec3& gridSize, const CoordFrame& localToVoxelSpace, const StaticMesh& staticMesh, MeshType meshType) = 0;
};


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_VOXELIZER_H
