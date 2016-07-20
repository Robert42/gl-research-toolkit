#include <glrt/dependencies.h>
#include <glrt/toolkit/zindex.h>
#include <glrt/glsl/math-cpp.h>
#include <gtest/gtest.h>
#include <test-vectors.h>
#include <random>

using namespace glrt::glsl;
using glm::vec2;
using glm::vec3;
using glm::vec4;



TEST(shader_library, plane_clamp_point_to_front_side)
{
  Plane plane = plane_from_normal(vec3(0,0,1), 0.f);
  EXPECT_VEC_EQ(clamp_point_to_front_side(plane, vec3(0, 0, -1)), vec3(0));
  EXPECT_VEC_EQ(clamp_point_to_front_side(plane, vec3(0, 0, 0)), vec3(0, 0, 0));
  EXPECT_VEC_EQ(clamp_point_to_front_side(plane, vec3(0, 0, 1)), vec3(0, 0, 1));
}


TEST(shader_library, cone_intersects_sphere)
{
  Cone cone = cone_from_ray_tan_angle(vec3(0), normalize(vec3(1, 1, 0)), 1.f);

  Sphere sphere;
  sphere.origin = vec3(10, -3, 0);
  sphere.radius = 3;
  EXPECT_TRUE(cone_intersects_sphere(cone, sphere));

  sphere.radius = 2.999f;
  EXPECT_FALSE(cone_intersects_sphere(cone, sphere));

  sphere.radius = 4;
  EXPECT_TRUE(cone_intersects_sphere(cone, sphere));

  sphere.origin = vec3(-4.1, 0, 0);
  EXPECT_FALSE(cone_intersects_sphere(cone, sphere));
}


TEST(shader_library, cone_from_point_to_sphere)
{
  const float epsilon = 2.e-4f;
  Sphere sphere;

  sphere.origin = vec3(10, 42, -5);
  sphere.radius = 5;

  Cone cone = cone_from_point_to_sphere(vec3(0, 42, -5), sphere);
  EXPECT_VEC_NEAR(cone.direction, vec3(1, 0, 0), epsilon);
  EXPECT_VEC_NEAR(cone.origin, vec3(0, 42, -5), epsilon);
  EXPECT_NEAR(cone_half_angle(cone), glm::radians(30.f), epsilon);

  cone = cone_from_point_to_sphere(vec3(9, 42, -5), sphere);
  EXPECT_VEC_NEAR(cone.direction, vec3(1, 0, 0), epsilon);
  EXPECT_VEC_NEAR(cone.origin, vec3(9, 42, -5), epsilon);
  EXPECT_NEAR(cone_half_angle(cone), glm::radians(89.f), epsilon);
}


TEST(shader_library, voxelIndexFromScalarIndex)
{
  glm::ivec3 size(256);
  for(int x=0; x<size.x; ++x)
    for(int y=0; y<size.y; ++y)
      for(int z=0; z<size.z; ++z)
        EXPECT_EQ(voxelIndexFromScalarIndex(x + y*size.x + z*size.x*size.y, size), glm::ivec3(x, y, z));

  size = glm::ivec3(125, 23, 7);
  for(int x=0; x<size.x; ++x)
    for(int y=0; y<size.y; ++y)
      for(int z=0; z<size.z; ++z)
        EXPECT_EQ(voxelIndexFromScalarIndex(x + y*size.x + z*size.x*size.y, size), glm::ivec3(x, y, z));
}



TEST(shader_library, triangle_ray_intersection_unclamped)
{
  Ray ray;
  ray.origin = vec3(0);
  ray.direction = vec3(0, 0, 1);

  for(float z : {-42.f, -10.f, -1.f, 0.f, 1.f, 10.f, 42.f})
  {
    vec3 triangle_with_hit[3] = {vec3(-1.5, -1, z), vec3(0.5, 1, z), vec3(0.5, -1, z)};
    vec3 triangle_without_hit[3] = {vec3(1, -1, z), vec3(2, 1, z), vec3(2, -1, z)};

    EXPECT_TRUE(triangle_ray_intersection_unclamped(ray, triangle_with_hit[0], triangle_with_hit[1], triangle_with_hit[2], 1.e-5f));
    EXPECT_FALSE(triangle_ray_intersection_unclamped(ray, triangle_without_hit[0], triangle_without_hit[1], triangle_without_hit[2], 1.e-5f));
  }
}



TEST(shader_library, plane_distance)
{
  Plane plane;

  plane.normal = vec3(1, 0, 0);
  plane.d = 2;

  EXPECT_EQ(signed_distance_to(plane, glm::vec3(3, 10, 10000)), 1);
  EXPECT_EQ(signed_distance_to(plane, glm::vec3(1, 10, 10000)), -1);
  EXPECT_EQ(distance_to(plane, glm::vec3(1, 10, 10000)), 1);
}

TEST(shader_library, ray_distance)
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

TEST(shader_library, ray_nearest_point)
{
  Plane plane;
  plane.d = 4;
  plane.normal = vec3(0,0,1);

  EXPECT_EQ(nearest_point(plane, vec3(42, 23, -5)), vec3(42, 23, 4));
}

TEST(shader_library, raytracing_plane)
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

TEST(shader_library, projection_plane)
{
  Plane projection_plane;
  vec3 p[2];

  projection_plane.normal = vec3(0, 0, 1);
  projection_plane.d = 1;


  p[0] = vec3(84, 46, 2);
  p[1] = vec3(-84, -46, -2);

  for(int i=0; i<2; ++i)
  {
    p[i] = perspective_projection_unclamped(projection_plane, vec3(0), p[i]);
    ASSERT_VEC_EQ(p[i], vec3(42, 23, 1));
  }


  projection_plane.normal = vec3(-0.794387, 0.571432, -0.205950);
  projection_plane.d = 1;
  p[0] = vec3(-0.150000, 0.317287, 0.569994);
  p[1] = vec3(-0.150000, 0.087474, 0.762831);
  for(int i=0; i<2; ++i)
    p[i] = perspective_projection_unclamped(projection_plane, vec3(0), p[i]);
  EXPECT_VEC_NEAR(p[0], vec3(-0.819333, 1.733090, 3.113433), 1.e-5);
  EXPECT_VEC_NEAR(p[1], vec3(-12.459891, 7.266071, 63.365238), 0.001f);
}
