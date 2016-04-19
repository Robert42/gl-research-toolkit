#include <glrt/scene/resources/voxel-data.h>

namespace glrt {
namespace scene {
namespace resources {


void VoxelData::initSize(const glm::vec3& meshBoundingBoxMin, const glm::vec3& meshBoundingBoxMax, float extend, int maxSize, float voxelsPerMeter)
{
  int minSize = int(glm::ceilPowerOfTwo<int>(int(glm::ceil(extend*2.f))));
  maxSize = int(glm::ceilPowerOfTwo<int>(maxSize));

  Q_ASSERT(maxSize >= minSize);
  Q_ASSERT(glm::all(glm::lessThanEqual(meshBoundingBoxMin, meshBoundingBoxMax)));

  glm::vec3 meshSize = meshBoundingBoxMax-meshBoundingBoxMin;

  glm::ivec3 voxels = glm::ceil(voxelsPerMeter * meshSize + extend*2.f);
  voxels = glm::ceilPowerOfTwo(voxels);
  voxels = glm::clamp(voxels, glm::ivec3(minSize), glm::ivec3(maxSize));
  this->voxelCount = glm::ivec3(voxels);

  glm::vec3 scale = (glm::vec3(voxels)-extend*2.f) / meshSize;

  float uniformScaleFactor = INFINITY;

  for(int i=0; i<3; ++i)
    if(glm::isnan(scale[i]) || glm::isinf(scale[i]) || scale[i] <=0.f)
      scale[i] = 1.f;
    else
      uniformScaleFactor = glm::min(uniformScaleFactor, uniformScaleFactor);

  if(glm::isinf(uniformScaleFactor))
  {
    qWarning() << "uniformScaleFactor == inf";
    uniformScaleFactor = 1.f;
  }

  CoordFrame offsetWorldSpace(-meshBoundingBoxMin);
  CoordFrame scaleWorldToVoxel(glm::vec3(0), glm::quat(), uniformScaleFactor);
  CoordFrame offsetVoxelSpace(glm::vec3(-extend));

  this->worldToVoxelSpace = (offsetVoxelSpace * scaleWorldToVoxel * offsetWorldSpace).toMat4();
  this->gpuTextureHandle = 0;
}


} // namespace resources
} // namespace scene
} // namespace glrt
