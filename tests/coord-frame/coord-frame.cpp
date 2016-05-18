#include <glrt/scene/coord-frame.h>
#include <glrt/glsl/math-cpp.h>
#include <gtest/gtest.h>
#include <test-vectors.h>

using namespace glrt::glsl;

namespace glm
{
float distance(const glrt::scene::CoordFrame& a, const glrt::scene::CoordFrame& b)
{
  return sqrt(sq(a.scaleFactor-b.scaleFactor)+
              sq(a.position-b.position)+
              sq(a.orientation.x-a.orientation.x)+
              sq(a.orientation.y-a.orientation.y)+
              sq(a.orientation.z-a.orientation.z)+
              sq(a.orientation.w-a.orientation.w));
}
}

class coord_frame : public ::testing::Test
{
protected:
  glm::vec3 all_dimensions[6];

  const glrt::scene::CoordFrame a = glrt::scene::CoordFrame(glm::vec3(1, 2, 3),
                                                            glm::angleAxis(glm::radians(-90.f), glm::vec3(0, 0, 1)),
                                                            0.5f);
  const glrt::scene::CoordFrame b = glrt::scene::CoordFrame(glm::vec3(7, 8, 9),
                                                            glm::angleAxis(glm::radians(90.f), glm::vec3(1, 0, 0)),
                                                            3.f);

  const glm::mat4 ma = glm::mat4(0, -0.5f, 0, 0,
                                 0.5f,  0, 0, 0,
                                 0,  0, 0.5f, 0,
                                 1,  2, 3, 1);

  void SetUp() override
  {
    for(int i=0; i<6; ++i)
      all_dimensions[i][i/2] = (i%2==0 ? 1 : -1);
  }
};


TEST_F(coord_frame, transformations)
{
  EXPECT_VEC_EQ(a* glm::vec3(0), glm::vec3(1, 2, 3));
  EXPECT_VEC_EQ(b* glm::vec3(0), glm::vec3(7, 8, 9));

  EXPECT_VEC_EQ(a* glm::vec3(0, 0, 1), glm::vec3(1, 2, 3.5));
  EXPECT_VEC_EQ(b* glm::vec3(0, 0, 1), glm::vec3(7, 5, 9));
}

TEST_F(coord_frame, concatenation)
{
  EXPECT_FLOAT_EQ((a*b).scaleFactor, 1.5f);
  EXPECT_VEC_EQ((a*b).position, a*glm::vec3(7, 8, 9));
  EXPECT_FLOAT_EQ((a*b).orientation.x, (a.orientation*b.orientation).x);
  EXPECT_FLOAT_EQ((a*b).orientation.y, (a.orientation*b.orientation).y);
  EXPECT_FLOAT_EQ((a*b).orientation.z, (a.orientation*b.orientation).z);
  EXPECT_FLOAT_EQ((a*b).orientation.w, (a.orientation*b.orientation).w);

  EXPECT_VEC_EQ((a*b) * glm::vec3(0), glm::vec3(1, 2, 3) + glm::vec3(8, -7, 9)*.5f);
  EXPECT_VEC_EQ((a*b) * glm::vec3(0, 0, 1), glm::vec3(1, 2, 3) + glm::vec3(5, -7, 9)*.5f);

  for(int i=0; i<6; ++i)
    EXPECT_VEC_NEAR((a*b)*all_dimensions[i], a*(b*all_dimensions[i]), 1.e-6);
}

TEST_F(coord_frame, to_matrix)
{
  EXPECT_VEC_NEAR(ma[0], a.toMat4()[0], 1.e-7);
  EXPECT_VEC_NEAR(ma[1], a.toMat4()[1], 1.e-7);
  EXPECT_VEC_NEAR(ma[2], a.toMat4()[2], 1.e-7);
  EXPECT_VEC_NEAR(ma[3], a.toMat4()[3], 1.e-7);
}

TEST_F(coord_frame, from_matrix)
{
  EXPECT_VEC_EQ(glrt::scene::CoordFrame(ma), a);
}

TEST_F(coord_frame, invert)
{
  EXPECT_VEC_NEAR(a*a.inverse(), glrt::scene::CoordFrame(), 1.e-7);
  EXPECT_VEC_NEAR(a.inverse()*a, glrt::scene::CoordFrame(), 1.e-7);
  EXPECT_VEC_NEAR(b*b.inverse(), glrt::scene::CoordFrame(), 1.e-7);
  EXPECT_VEC_NEAR(b.inverse()*b, glrt::scene::CoordFrame(), 1.e-7);
}
