#ifndef GLRT_SCENE_RESOURCES_VOXELIZER_H
#define GLRT_SCENE_RESOURCES_VOXELIZER_H

#include <glrt/toolkit/uuid.h>
#include <glrt/scene/resources/static-mesh.h>
#include <glrt/scene/resources/texture-file.h>

namespace glrt {
namespace scene {
namespace resources {

class ResourceIndex;

class Voxelizer final
{
public:
  enum class FieldType : quint32
  {
    SIGNED_DISTANCE_FIELD,
  };

  ResourceIndex* resourceIndex = nullptr;

  struct Hints
  {
    TextureFile::Type voxelType = TextureFile::Type::FLOAT16;
    float extend = 1.f;
    int minSize = 4;
    int maxSize = 128;
    float voxelsPerMeter = 2;
    bool enabled = true;
  };

  Hints signedDistanceField;


  Voxelizer(ResourceIndex* resourceIndex);
  ~Voxelizer();

  static void registerAngelScriptAPI();

  void voxelize(const Uuid<StaticMesh>& staticMeshUuid);

private:
  void revoxelizeMesh(const Uuid<StaticMesh>& staticMeshUuid, const QString& staticMeshFileName, const QString& voxelFileName);
};

uint qHash(glrt::scene::resources::Voxelizer::FieldType type);


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_VOXELIZER_H