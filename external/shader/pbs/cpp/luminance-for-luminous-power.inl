#include <glrt/dependencies.h>

namespace pbs {

using namespace glrt;

// Table 12

inline glm::vec3 luminance_for_sphere(const glm::vec3& luminous_power, float radius)
{
  return luminous_power / (4.f * sq(radius * glm::pi<float>()));
}

inline glm::vec3 luminance_for_disk(const glm::vec3& luminous_power, float radius)
{
  return luminous_power / (sq(radius * glm::pi<float>()));
}

inline glm::vec3 luminance_for_tube(const glm::vec3& luminous_power, float radius, float width)
{
  return luminous_power / (2.f * glm::pi<float>() * radius * width + 4.f * sq(radius * glm::pi<float>()));
}

inline glm::vec3 luminance_for_rect(const glm::vec3& luminous_power, float width, float height)
{
  return luminous_power / (width * height * glm::pi<float>());
}


} // namespace pbs
