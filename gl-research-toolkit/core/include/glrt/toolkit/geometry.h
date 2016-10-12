#ifndef GLRT_GEOMETRY_H
#define GLRT_GEOMETRY_H

#include <glrt/dependencies.h>

namespace glrt {

template<typename T>
inline T voxelIndexForCoordinate(const glm::tvec3<T>& voxelCoordinate, const glm::tvec3<T>& gridSize)
{
  return voxelCoordinate.z * gridSize.y * gridSize.x + voxelCoordinate.y * gridSize.x + voxelCoordinate.x;
}


inline glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal)
{
  return glm::cross(vector, normal);
}

template<typename... args>
glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal_candidate_1 , const glm::vec3& normal_candidate_2, const args&... other_candidates)
{
  if(glm::abs(glm::dot(vector, normal_candidate_1)) <= glm::abs(glm::dot(vector, normal_candidate_2)))
  {
    return find_best_perpendicular(vector, normal_candidate_1, other_candidates...);
  }else
  {
    return find_best_perpendicular(vector, normal_candidate_2, other_candidates...);
  }
}

inline glm::vec3 find_best_perpendicular(const glm::vec3& vector)
{
  return find_best_perpendicular(vector, glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
}


inline glm::vec3 transform_point(const glm::mat4& t, const glm::vec3& point)
{
  glm::vec4 vector(point, 1);

  vector = t * vector;

  return vector.xyz() / vector.w;
}

inline glm::vec3 transform_point(const glm::mat4x3& t, const glm::vec3& point)
{
  glm::vec4 vector(point, 1);

  return t * vector;
}

inline glm::vec3 transform_direction(const glm::mat4& t, const glm::vec3& relative)
{
  glm::vec4 vector(relative, 0);

  vector = t * vector;

  return vector.xyz();
}

inline glm::vec3 transform_direction(const glm::mat4x3& t, const glm::vec3& relative)
{
  glm::vec4 vector(relative, 0);

  return t * vector;
}


} // namespace glrt

#endif // GLRT_GEOMETRY_H
