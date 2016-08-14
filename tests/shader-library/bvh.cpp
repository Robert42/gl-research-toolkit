#include <glrt/scene/resources/static-mesh.h>
#include <glrt/renderer/voxel-buffer.h>
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
using glrt::renderer::BVH;

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
  EXPECT_EQ(glrt::insertZeroBits(0x0),    quint32(0x0));
  EXPECT_EQ(glrt::insertZeroBits(0x1),    quint32(0x1));
  EXPECT_EQ(glrt::insertZeroBits(0x2),    quint32(0x8));
  EXPECT_EQ(glrt::insertZeroBits(0x3),    quint32(0x9));
  EXPECT_EQ(glrt::insertZeroBits(0x7),    quint32(0x49));
  EXPECT_EQ(glrt::insertZeroBits(0xf),    quint32(0x249));
  EXPECT_EQ(glrt::insertZeroBits(0x1f),   quint32(0x1249));
  EXPECT_EQ(glrt::insertZeroBits(0x3f),   quint32(0x9249));
  EXPECT_EQ(glrt::insertZeroBits(0x7f),   quint32(0x49249));
  EXPECT_EQ(glrt::insertZeroBits(0xff),   quint32(0x249249));
  EXPECT_EQ(glrt::insertZeroBits(0x1ff),  quint32(0x1249249));
  EXPECT_EQ(glrt::insertZeroBits(0x3ff),  quint32(0x9249249));

  // bits >= 1024 are ignored:
  EXPECT_EQ(glrt::insertZeroBits(0xfff),  quint32(0x9249249));
  EXPECT_EQ(glrt::insertZeroBits(0x1fff),  quint32(0x9249249));
}

TEST(shader_library, bvh_generation)
{
  const quint16 num_leaves = 5;
  BoundingSphere leaves_bounding_spheres[num_leaves];
  quint32 leaves_z_indices[num_leaves];

  for(quint32 i=0; i<num_leaves; ++i)
  {
    leaves_bounding_spheres[i] = BoundingSphere{glm::vec3(i*4.f, 0.f, 0.f), 1.f};
    leaves_z_indices[i] = glrt::insertZeroBits((i*1023)/num_leaves);
  }

  BVH bvh(leaves_bounding_spheres, leaves_z_indices, num_leaves);

  BoundingSphere node_bounding_spheres[num_leaves-1];
  BVH::InnerNode inner_nodes[num_leaves-1];

  ASSERT_NE(2, bvh.findSplit(2, 4));

  bvh.updateTreeCPU(node_bounding_spheres, inner_nodes);
}
