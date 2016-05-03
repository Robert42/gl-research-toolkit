#ifndef GLRT_SCENE_RESOURCES_CPUVOXELIZERIMPLEMENTATION_H
#define GLRT_SCENE_RESOURCES_CPUVOXELIZERIMPLEMENTATION_H

#include <glrt/scene/resources/voxelizer.h>

namespace glrt {
namespace scene {
namespace resources {

class CpuVoxelizerImplementation : public Voxelizer::Implementation
{
public:
  static void voxelizeToSphere(QVector<float>& data, const glm::ivec3& gridSize, const glm::vec3& origin, float radius);
  static void voxelizeToSphere(QVector<float>& data, const glm::ivec3& gridSize);
  static void voxeliseMesh(QVector<float>& data, const glm::ivec3& gridSize, const CoordFrame& localToVoxelSpace, const StaticMesh& staticMesh, const Material& material);
  utilities::GlTexture distanceField(const glm::ivec3& gridSize, const CoordFrame& localToVoxelSpace, const StaticMesh& staticMesh, const Material& material) override;
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_CPUVOXELIZERIMPLEMENTATION_H
