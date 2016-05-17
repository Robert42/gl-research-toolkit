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
  static void voxelizeGradient(QVector<float>& data, const glm::ivec3& gridSize, float value_for_min_z, float value_for_max_z);
  static void voxeliseMesh(QVector<float>& data, const glm::ivec3& gridSize, const CoordFrame& localToVoxelSpace, TriangleArray staticMesh, MeshType meshType);
  utilities::GlTexture distanceField(const glm::ivec3& gridSize, const CoordFrame& localToVoxelSpace, const TriangleArray& staticMesh, MeshType meshType) override;
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_CPUVOXELIZERIMPLEMENTATION_H
