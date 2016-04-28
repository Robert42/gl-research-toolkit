#ifndef GLRT_SCENE_RESOURCES_VOXELFILE_H
#define GLRT_SCENE_RESOURCES_VOXELFILE_H

#include <glrt/scene/resources/voxelizer.h>
#include <glrt/scene/coord-frame.h>

namespace glrt {
namespace scene {
namespace resources {

class VoxelFile
{
public:
  VoxelFile();
  ~VoxelFile();

  struct Header
  {
    quint64 magicNumber = VoxelFile::magicNumber();
    quint16 headerLength = sizeof(Header);
    quint16 metaDataLength = sizeof(MetaData);
    quint16 numVoxelFiles = 0;
    quint16 _padding = 0;
    QUuid meshValidationUuid;
  };

  struct MetaData
  {
    Voxelizer::FieldType fieldType = Voxelizer::FieldType::SIGNED_DISTANCE_FIELD;
    float factor = 1.f; // reserved for future usage (in case uint8 is used for signed distacne fields -- currently unused and must be 1)
    float offset = 0.f; // reserved for future usage (in case uint8 is used for signed distacne fields -- currently unused and must be 0)
    quint32 _padding[2] = {0,0};
    glm::ivec3 gridSize = glm::vec3(0);
    quint32 _padding2 = 0;
    CoordFrame localToVoxelSpace;
  };

  static quint64 magicNumber();

  Uuid<StaticMesh> meshUuid;
  QMap<QString, MetaData> textureFiles;

  void load(const QFileInfo& fileInfo, const Uuid<StaticMesh>& meshUuid);
  void save(const QFileInfo& fileInfo);
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_VOXELFILE_H
