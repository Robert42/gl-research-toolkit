#ifndef GLRT_GEOMETRY_H
#define GLRT_GEOMETRY_H

#include <glrt/dependencies.h>

namespace glrt {

glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal);

template<typename... args>
glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal_candidate_1 , const glm::vec3& normal_candidate_2, const args&... other_candidates)
{
  if(glm::abs(glm::dot(vector, normal_candidate_1)) < glm::abs(glm::dot(vector, normal_candidate_2)))
  {
    return find_best_perpendicular(vector, normal_candidate_1, other_candidates...);
  }else
  {
    return find_best_perpendicular(vector, normal_candidate_2, other_candidates...);
  }
}

glm::vec3 find_best_perpendicular(const glm::vec3& vector);

} // namespace glrt

#endif // GLRT_GEOMETRY_H
