#include "cpuvoxelizerimplementation.h"

#include <openvdb/triangle-distance.h>

namespace glrt {
namespace scene {
namespace resources {


inline int coordToIndex(int x, int y, int z, const glm::ivec3& gridSize)
{
  return x + gridSize.x * (y + gridSize.y * z);
}

inline glm::vec3 centerPointOfVoxel(int x, int y, int z)
{
  return glm::vec3(x,y,z)+0.5f;
}

void CpuVoxelizerImplementation::voxelizeToSphere(QVector<float>& data, const glm::ivec3& gridSize, const glm::vec3& origin, float radius)
{
#pragma omp parallel for
  for(int z=0; z<gridSize.z; ++z)
    for(int y=0; y<gridSize.y; ++y)
      for(int x=0; x<gridSize.x; ++x)
          data[coordToIndex(x, y, z, gridSize)] = distance(centerPointOfVoxel(x,y,z), origin) - radius;
}

void CpuVoxelizerImplementation::voxelizeToSphere(QVector<float>& data, const glm::ivec3& gridSize)
{
  float radius = glm::min(gridSize.x, glm::min(gridSize.y, gridSize.z))*0.5f - 1.f;

  voxelizeToSphere(data, gridSize, glm::vec3(gridSize)*.5f, radius);
}

void CpuVoxelizerImplementation::voxelizeGradient(QVector<float>& data, const glm::ivec3& gridSize, float value_for_min_z, float value_for_max_z)
{
  const float factor = (value_for_max_z - value_for_min_z) / float(glm::max(1, gridSize.z - 1));

#pragma omp parallel for
  for(int z=0; z<gridSize.z; ++z)
    for(int y=0; y<gridSize.y; ++y)
      for(int x=0; x<gridSize.x; ++x)
        data[coordToIndex(x, y, z, gridSize)] = z * factor + value_for_min_z;
}

void CpuVoxelizerImplementation::voxeliseMesh(QVector<float>& data, const glm::ivec3& gridSize, const CoordFrame& localToVoxelSpace, TriangleArray<> staticMesh, MeshType meshType)
{
  bool twoSided = meshType == MeshType::TWO_SIDED;

  QVector<glm::vec3> vertices_array;

  const int num_vertices = staticMesh.length();

#pragma omp parallel for
    for(int i=0; i<num_vertices; i++)
      staticMesh[i] = localToVoxelSpace.transform_point(staticMesh[i]);

#pragma omp parallel for
  for(int z=0; z<gridSize.z; ++z)
    for(int y=0; y<gridSize.y; ++y)
      for(int x=0; x<gridSize.x; ++x)
      {
        float best_positive_d = INFINITY;
        float best_negative_d = -INFINITY;
        float best_d_abs = INFINITY;

        for(int i=0; i<num_vertices; i+=3)
        {
          Q_ASSERT(i+2<num_vertices);

          const glm::vec3 p = centerPointOfVoxel(x,y,z);
          const glm::vec3& v0 = staticMesh[i];
          const glm::vec3& v1 = staticMesh[i+1];
          const glm::vec3& v2 = staticMesh[i+2];

          glm::vec3 uvw;
          glm::vec3 closestPoint = openvdb::math::closestPointOnTriangleToPoint(v0, v1, v2, p, uvw);

          float d_abs = distance(closestPoint, p);
          float d = -glm::faceforward(glm::vec3(d_abs,0,0), glm::cross(v1-v0, v2-v0), p-closestPoint).x;

          best_d_abs = glm::min(d_abs, best_d_abs);
          best_positive_d = d >= 0 ? glm::min(d, best_positive_d) : best_positive_d;
          best_negative_d = d <= 0 ? glm::max(d, best_negative_d) : best_negative_d;
        }

        float best_d;

        if(twoSided)
        {
          best_d = best_d_abs;
        }else
        {
          if(glm::abs(best_negative_d) + 1.e-5f < best_positive_d)
            best_d = best_negative_d;
          else
            best_d = best_positive_d;
        }

        data[coordToIndex(x, y, z, gridSize)] = best_d;
      }
}

utilities::GlTexture CpuVoxelizerImplementation::distanceField(const glm::ivec3& gridSize, const CoordFrame& localToVoxelSpace, const TriangleArray<>& staticMesh, MeshType meshType)
{
  qWarning() << "Using the CPU voxelizer implementation";

  utilities::GlTexture texture;

  utilities::GlTexture::TextureAsFloats asFloats(gridSize, 1);

  QVector<float>& data = asFloats.textureData;

//  voxelizeGradient(data, gridSize, -1.f, 1.f);
//  voxelizeToSphere(data, gridSize);
  voxeliseMesh(data, gridSize, localToVoxelSpace, staticMesh, meshType);

  texture.fromFloats(asFloats);

  return texture;
}


} // namespace resources
} // namespace scene
} // namespace glrt
