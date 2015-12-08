#include <glrt/glsl/geometry/raytracing.h>


int main(int argc, char** argv)
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  glrt::glsl::Plane plane;

  plane.normal = glm::vec3(1, 0, 0);
  plane.d = 2;

  Q_ASSERT(glrt::glsl::signed_distance_to(plane, glm::vec3(3, 10, 10000)) == 1);
  Q_ASSERT(glrt::glsl::signed_distance_to(plane, glm::vec3(1, 10, 10000)) == -1);

  return 0;
}
