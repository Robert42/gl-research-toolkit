#include <glrt/toolkit/geometry.h>

namespace glrt {

glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal)
{
  return glm::perp(vector, normal);
}

glm::vec3 find_best_perpendicular(const glm::vec3& vector)
{
  return find_best_perpendicular(vector, glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
}

} // namespace glrt

