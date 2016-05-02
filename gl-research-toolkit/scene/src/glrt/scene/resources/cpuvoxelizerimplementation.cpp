#include "cpuvoxelizerimplementation.h"

#include <openvdb/triangle-distance.h>

namespace glrt {
namespace scene {
namespace resources {


void CpuVoxelizerImplementation::voxelizeToSphere(QVector<float>& data, const glm::ivec3& gridSize, const glm::vec3& origin, float radius)
{
#pragma omp parallel for
  for(int z=0; z<gridSize.z; ++z)
    for(int y=0; y<gridSize.y; ++y)
      for(int x=0; x<gridSize.x; ++x)
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

  if(staticMesh.isIndexed())
  {
#pragma omp parallel for
  for(int z=0; z<gridSize.z; ++z)
    for(int y=0; y<gridSize.y; ++y)
      for(int x=0; x<gridSize.x; ++x)
        for(quint16 i=0; i<staticMesh.indices.length(); i+=3)
        {
          const glm::vec3& v0 = staticMesh.vertices[i].position;
          const glm::vec3& v1 = staticMesh.vertices[i+1].position;
          const glm::vec3& v2 = staticMesh.vertices[i+2].position;


        }
  }

  texture.fromFloats(asFloats);

  return texture;
}


} // namespace resources
} // namespace scene
} // namespace glrt
