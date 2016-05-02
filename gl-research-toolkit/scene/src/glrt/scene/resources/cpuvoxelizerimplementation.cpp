#include "cpuvoxelizerimplementation.h"

namespace glrt {
namespace scene {
namespace resources {


void CpuVoxelizerImplementation::voxelizeToSphere(QVector<float>& data, const glm::ivec3& gridSize, const glm::vec3& origin, float radius)
{
#pragma omp parallel for
    for(int x=0; x<gridSize.x; ++x)
      for(int y=0; y<gridSize.y; ++y)
        for(int z=0; z<gridSize.z; ++z)
          data[x + gridSize.x * (y + gridSize.y * z)] = distance(glm::vec3(x,y,z)+0.5f, origin) - radius;
}

void CpuVoxelizerImplementation::voxelizeToSphere(QVector<float>& data, const glm::ivec3& gridSize)
{
  float radius = glm::min(gridSize.x, glm::min(gridSize.y, gridSize.z))*0.5f - 1.f;

  voxelizeToSphere(data, gridSize, glm::vec3(gridSize)*.5f, radius);
}

utilities::GlTexture CpuVoxelizerImplementation::distanceField(const glm::ivec3& gridSize, const CoordFrame& localToVoxelSpace, const StaticMesh& staticMesh, const Material& material)
{
  utilities::GlTexture texture;

  utilities::GlTexture::TextureAsFloats asFloats(gridSize, 1);

  voxelizeToSphere(asFloats.textureData, gridSize);

  texture.fromFloats(asFloats);

  return texture;
}


} // namespace resources
} // namespace scene
} // namespace glrt
