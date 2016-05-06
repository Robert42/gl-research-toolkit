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

void CpuVoxelizerImplementation::voxeliseMesh(QVector<float>& data, const glm::ivec3& gridSize, const CoordFrame& localToVoxelSpace, const StaticMesh& staticMesh, MeshType meshType)
{
  bool twoSided = meshType == MeshType::TWO_SIDED;

  QVector<glm::vec3> vertices_array;
  glm::vec3* vertices = nullptr;

  if(staticMesh.isIndexed())
  {
    const int n = staticMesh.indices.length();
    vertices_array.resize(n);
    vertices = vertices_array.data();

#pragma omp parallel for
    for(int i=0; i<n; i++)
      vertices[i] = localToVoxelSpace.transform_point(staticMesh.vertices[staticMesh.indices[i]].position);
  }else
  {
    const int n = staticMesh.vertices.length();
    vertices_array.resize(n);
    vertices = vertices_array.data();

#pragma omp parallel for
    for(int i=0; i<n; i++)
      vertices[i] = localToVoxelSpace.transform_point(staticMesh.vertices[i].position);
  }

  const int num_vertices = vertices_array.length();

#pragma omp parallel for
  for(int z=0; z<gridSize.z; ++z)
    for(int y=0; y<gridSize.y; ++y)
      for(int x=0; x<gridSize.x; ++x)
      {
        float best_d = INFINITY;
        float best_d_abs = INFINITY;

        for(int i=0; i<num_vertices; i+=3)
        {
          Q_ASSERT(i+2<num_vertices);

          const glm::vec3 p = centerPointOfVoxel(x,y,z);
          const glm::vec3& v0 = vertices[i];
          const glm::vec3& v1 = vertices[i+1];
          const glm::vec3& v2 = vertices[i+2];

          glm::vec3 uvw;
          glm::vec3 closestPoint = openvdb::math::closestPointOnTriangleToPoint(v0, v1, v2, p, uvw);

          float d_abs = distance(closestPoint, p);
          float d = -glm::faceforward(glm::vec3(d_abs,0,0), glm::cross(v1-v0, v2-v0), p-closestPoint).x;

          // Add a bias, to prefer faces with positive values
          // This way, if there are two polygons (backfaced and frontfaced) with the same distance, the positive is preferred
          float sign_bias = d > 0 ? 1.e-5f : 0.f;

          if(Q_UNLIKELY(best_d_abs + sign_bias > d_abs))
          {
            best_d = d;
            best_d_abs = d_abs;
          }
        }
        if(Q_UNLIKELY(twoSided))
          best_d = best_d_abs;
        data[coordToIndex(x, y, z, gridSize)] = best_d;
      }
}

utilities::GlTexture CpuVoxelizerImplementation::distanceField(const glm::ivec3& gridSize, const CoordFrame& localToVoxelSpace, const StaticMesh& staticMesh, MeshType meshType)
{
  utilities::GlTexture texture;

  utilities::GlTexture::TextureAsFloats asFloats(gridSize, 1);

  QVector<float>& data = asFloats.textureData;

//  voxelizeToSphere(data, gridSize);
  voxeliseMesh(data, gridSize, localToVoxelSpace, staticMesh, meshType);

  texture.fromFloats(asFloats);

  return texture;
}


} // namespace resources
} // namespace scene
} // namespace glrt
