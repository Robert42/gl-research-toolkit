#include <glrt/dependencies.h>
#include <glrt/glsl/math.h>

using namespace glrt::glsl;
using glm::vec2;
using glm::vec3;
using glm::vec4;

#define EXPECT_NEAR(value, expected) if(distance(value, expected) > 0.00001f){qCritical() << "Expected"<<expected<<"got"<<value<<"\nExpression for the tested value: " <<#value<<"\nExpression for the expected value: "<<#expected<<"\n";Q_UNREACHABLE();}
#define EXPECT_EQ(value, expected) if(value!=expected){qCritical() << "Expected"<<expected<<"got"<<value<<"\nExpression for the tested value: " <<#value<<"\nExpression for the expected value: "<<#expected<<"\n";Q_UNREACHABLE();}
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

void test_ray_nearest_point()
{
  Plane plane;
  plane.d = 4;
  plane.normal = vec3(0,0,1);

  EXPECT_EQ(nearest_point(plane, vec3(42, 23, -5)), vec3(42, 23, 4));
}

void test_raytracing_plane()
{
  Plane plane1;
  plane1.d = 4;
  plane1.normal = vec3(0,0,1);

  Plane plane2;
  plane2.d = -4;
  plane2.normal = vec3(0,0,-1);

  Ray ray1;
  ray1.origin = vec3(0, 0, -12);
  ray1.direction = vec3(0, 0, 1);

  Ray ray2;
  ray2.origin = vec3(0, 0, -12);
  ray2.direction = vec3(0, 0, -1);

  Ray ray3;
  ray3.origin = vec3(0, 0, 7);
  ray3.direction = vec3(0, 0, 1);

  Ray ray4;
  ray4.origin = vec3(0, 0, 7);
  ray4.direction = vec3(0, 0, -1);

  EXPECT_EQ(intersection_distance(plane1, ray1), 16);
  EXPECT_EQ(intersection_distance(plane1, ray2),-16);
  EXPECT_EQ(intersection_distance(plane1, ray3),-3);
  EXPECT_EQ(intersection_distance(plane1, ray4), 3);

  EXPECT_EQ(intersection_distance(plane2, ray1), 16);
  EXPECT_EQ(intersection_distance(plane2, ray2),-16);
  EXPECT_EQ(intersection_distance(plane2, ray3),-3);
  EXPECT_EQ(intersection_distance(plane2, ray4), 3);
}

void test_raytracing_sphere()
{
  Ray ray;
  Sphere sphere;

  ray.direction = vec3(1, 0, 0);
  ray.origin = vec3(0, 42, 0);

  sphere.origin = vec3(5, 8, 0);
  sphere.radius = 3;

  EXPECT_EQ(nearest_point_on_sphere_unclamped(sphere, ray), vec3(5, 11, 0));

  sphere.origin = vec3(-0.700000, 0.500000, 1.600000);
  sphere.radius = 0.1125;

  ray.direction = vec3(-0.150159, 0.741311, 0.654148);
  ray.origin = vec3(-0.384320, -0.902267, 0.376229);
}

void test_raytracing_rect()
{
  Ray ray;
  Rect rect;

  rect.origin = vec3(1000, 1000, 42);
  rect.half_width = 512;
  rect.half_height = 384;
  rect.tangent1 = vec3(1,0,0);
  rect.tangent2 = vec3(0,1,0);

  vec3 nearest_point;


  ray.origin = vec3(0);
  ray.direction = vec3(0, 0, -1);

  bool is_intersecting = nearest_point_on_rect(rect, ray, nearest_point);
  EXPECT_EQ(nearest_point, vec3(1000-512, 1000-384, 42));
  EXPECT_FALSE(is_intersecting);


  ray.origin = vec3(0);
  ray.direction = vec3(0, 0, 1);

  is_intersecting = nearest_point_on_rect(rect, ray, nearest_point);
  EXPECT_EQ(nearest_point, vec3(1000-512, 1000-384, 42));
  EXPECT_TRUE(is_intersecting);


  ray.origin = vec3(10000, 10000, 0);
  ray.direction = vec3(0, 0, 1);

  is_intersecting = nearest_point_on_rect(rect, ray, nearest_point);
  EXPECT_EQ(nearest_point, vec3(1000+512, 1000+384, 42));
  EXPECT_TRUE(is_intersecting);


  rect.origin = vec3(0, 0, 1);
  rect.half_width = 0.3;
  rect.half_height = 0.2;
  rect.tangent1 = vec3(-1,0,0);
  rect.tangent2 = vec3(0,1,0);

  ray.origin = vec3(1, 0, 0);
  ray.direction = normalize(vec3(-0.1, 0, 1));

  is_intersecting = nearest_point_on_rect(rect, ray, nearest_point);
  EXPECT_EQ(nearest_point, vec3(0.3, 0, 1));
  EXPECT_TRUE(is_intersecting);

  ray.direction = normalize(vec3(-0.1, 0, -1));

  is_intersecting = nearest_point_on_rect(rect, ray, nearest_point);
  EXPECT_EQ(nearest_point, vec3(0.3, 0, 1));
  EXPECT_FALSE(is_intersecting);
}


int main(int argc, char** argv)
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  test_plane_distance();
  test_ray_distance();
  test_ray_nearest_point();
  test_raytracing_plane();
  test_raytracing_sphere();
  test_raytracing_rect();

  return 0;
}
