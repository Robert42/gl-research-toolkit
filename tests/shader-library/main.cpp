#include <glrt/glsl/geometry/raytracing.h>

using namespace glrt::glsl;
using glm::vec2;
using glm::vec3;
using glm::vec4;

#define EXPECT_EQ(value, expected) if(value!=expected){qCritical() << "Expected"<<expected<<"got"<<value;Q_UNREACHABLE();}
#define EXPECT_TRUE(value) EXPECT_EQ(value, true);
#define EXPECT_FALSE(value) EXPECT_EQ(value, false);

void test_plane_distance()
{
  Plane plane;

  plane.normal = vec3(1, 0, 0);
  plane.d = 2;

  Q_ASSERT(signed_distance_to(plane, glm::vec3(3, 10, 10000)) == 1);
  Q_ASSERT(signed_distance_to(plane, glm::vec3(1, 10, 10000)) == -1);
  Q_ASSERT(distance_to(plane, glm::vec3(1, 10, 10000)) == 1);
}

void test_ray_distance()
{
  Ray ray1;

  ray1.direction = vec3(0, 1, 0);
  ray1.origin = vec3(0);

  EXPECT_EQ(distance_to(ray1, vec3(3, 0, 0)), 3);
  EXPECT_EQ(distance_to(ray1, vec3(-3, 0, 0)), 3);
  EXPECT_EQ(distance_to(ray1, vec3(0, 5, 0)), 0);
  EXPECT_EQ(distance_to(ray1, vec3(0, -5, 0)), 5);
  EXPECT_EQ(distance_to(ray1, vec3(0, 0, 0)), 0);

  EXPECT_EQ(distance_to_unclamped(ray1, vec3(3, 0, 0)), 3);
  EXPECT_EQ(distance_to_unclamped(ray1, vec3(-3, 0, 0)), 3);
  EXPECT_EQ(distance_to_unclamped(ray1, vec3(0, 5, 0)), 0);
  EXPECT_EQ(distance_to_unclamped(ray1, vec3(0, -5, 0)), 0);
  EXPECT_EQ(distance_to_unclamped(ray1, vec3(0, 0, 0)), 0);

  EXPECT_EQ(sq_distance_to(ray1, vec3(3, 0, 0)), 9);
  EXPECT_EQ(sq_distance_to(ray1, vec3(-3, 0, 0)), 9);
  EXPECT_EQ(sq_distance_to(ray1, vec3(0, 5, 0)), 0);
  EXPECT_EQ(sq_distance_to(ray1, vec3(0, -5, 0)), 25);
  EXPECT_EQ(sq_distance_to(ray1, vec3(0, 0, 0)), 0);

  EXPECT_EQ(sq_distance_to_unclamped(ray1, vec3(3, 0, 0)), 9);
  EXPECT_EQ(sq_distance_to_unclamped(ray1, vec3(-3, 0, 0)), 9);
  EXPECT_EQ(sq_distance_to_unclamped(ray1, vec3(0, 5, 0)), 0);
  EXPECT_EQ(sq_distance_to_unclamped(ray1, vec3(0, -5, 0)), 0);
  EXPECT_EQ(sq_distance_to_unclamped(ray1, vec3(0, 0, 0)), 0);
}

void test_raytracing_sphere()
{
  Ray ray;
  Sphere small;

  ray.direction = vec3(1, 0, 0);
  ray.origin = vec3(0);


}


int main(int argc, char** argv)
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  test_plane_distance();
  test_ray_distance();
  test_raytracing_sphere();

  return 0;
}
