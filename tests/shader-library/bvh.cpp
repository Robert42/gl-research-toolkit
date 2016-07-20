#include <glrt/scene/resources/static-mesh.h>
#include <glrt/toolkit/zindex.h>
#include <glrt/glsl/math-cpp.h>
#include <gtest/gtest.h>
#include <test-vectors.h>
#include <random>

using namespace glrt::glsl;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using glrt::scene::resources::BoundingSphere;

TEST(shader_library, bounding_sphere_merging)
{
  BoundingSphere a, b, c;

  a.center = vec3(1, 2, 3);
  a.radius = 10.f;
  b.center = vec3(2, 3, 4);
  b.radius = 2;

  EXPECT_VEC_EQ((a|b).center, a.center);
  EXPECT_FLOAT_EQ((a|b).radius, a.radius);

  EXPECT_VEC_EQ((a|a).center, a.center);
  EXPECT_FLOAT_EQ((a|a).radius, a.radius);

  EXPECT_VEC_EQ((b|a).center, a.center);
  EXPECT_FLOAT_EQ((b|a).radius, a.radius);

  EXPECT_VEC_EQ((b|b).center, b.center);
  EXPECT_FLOAT_EQ((b|b).radius, b.radius);
}


TEST(shader_library, bvh_split)
{
  EXPECT_EQ(glm::highestBitValue(1025), 1024);
}


TEST(shader_library, zindex)
{
  EXPECT_EQ(glrt::insertZeroBits(0x0),    0x0);
  EXPECT_EQ(glrt::insertZeroBits(0x1),    0x1);
  EXPECT_EQ(glrt::insertZeroBits(0x2),    0x8);
  EXPECT_EQ(glrt::insertZeroBits(0x3),    0x9);
  EXPECT_EQ(glrt::insertZeroBits(0x7),    0x49);
  EXPECT_EQ(glrt::insertZeroBits(0xf),    0x249);
  EXPECT_EQ(glrt::insertZeroBits(0x1f),   0x1249);
  EXPECT_EQ(glrt::insertZeroBits(0x3f),   0x9249);
  EXPECT_EQ(glrt::insertZeroBits(0x7f),   0x49249);
  EXPECT_EQ(glrt::insertZeroBits(0xff),   0x249249);
  EXPECT_EQ(glrt::insertZeroBits(0x1ff),  0x1249249);
  EXPECT_EQ(glrt::insertZeroBits(0x3ff),  0x9249249);

  // bits >= 1024 are ignored:
  EXPECT_EQ(glrt::insertZeroBits(0xfff),  0x9249249);
  EXPECT_EQ(glrt::insertZeroBits(0x1fff),  0x9249249);
}
