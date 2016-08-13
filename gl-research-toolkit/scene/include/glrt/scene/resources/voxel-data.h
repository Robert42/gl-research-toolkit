#ifndef GLRT_SCENE_RESOURCES_VOXELDATA_H
#define GLRT_SCENE_RESOURCES_VOXELDATA_H

#include <glrt/dependencies.h>
#include <glrt/scene/coord-frame.h>
#include <glrt/scene/resources/declarations.h>
#include <glrt/scene/resources/voxelizer.h>

namespace glrt {
namespace scene {

class VoxelDataComponent;

namespace resources {

struct VoxelBoundingBox
{
  glm::mat4 worldToVoxelSpace = glm::mat4(1);
  glm::ivec3 voxelCount = glm::ivec3(0);

  VoxelBoundingBox(){}
  VoxelBoundingBox(const VoxelDataComponent& c);
};

struct VoxelData
{
  CoordFrame localToVoxelSpace;
  glm::ivec3 voxelCount = glm::ivec3(1);
  quint64 gpuTextureHandle = 0;

  CoordFrame worldToVoxelSpaceCoordFrame(const CoordFrame& localToWorldSpace) const;
  glm::mat4x3 worldToVoxelSpaceMatrix4x3(const CoordFrame& localToWorldSpace) const;
  glm::mat4 worldToVoxelSpaceMatrix4(const CoordFrame& localToWorldSpace) const;
  BoundingSphere worldSpaceBoundignSphere(const CoordFrame& localToWorldSpace) const;
};

struct VoxelIndex
{
  CoordFrame localToVoxelSpace;
  glm::ivec3 gridSize = glm::ivec3(0);
  Uuid<Texture> texture3D;
  float factor = 1.f; // reserved for future usage (in case uint8 is used for signed distacne fields -- currently unused and must be 1)
  float offset = 0.f; // reserved for future usage (in case uint8 is used for signed distacne fields -- currently unused and must be 0)
  bool voxelizedAsScenery;
  BoundingSphere boundingSphere;

  VoxelData toData(ResourceManager& resourceManager) const;
};


struct VoxelUniformDataBlock
{
  glm::vec3 globalWorldToVoxelMatrix_col0;
  int voxelCount_x;
  glm::vec3 globalWorldToVoxelMatrix_col1;
  int voxelCount_y;
  glm::vec3 globalWorldToVoxelMatrix_col2;
  int voxelCount_z;
  glm::vec3 globalWorldToVoxelMatrix_col3;
  float globalWorldToVoxelFactor;

  quint64 texture;
  padding<quint64, 1> _padding1;
};


} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_VOXELDATA_H
