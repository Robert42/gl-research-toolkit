#include <glrt/scene/coord-frame.h>
#include <glrt/glsl/math.h>

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

#define EXPECT_NEAR_EPSILON(value, expected, epsilon) if(glm::distance(value, expected) > epsilon){qCritical() << "Expected"<<expected<<"got"<<value<<"\nExpression for the tested value: " <<#value<<"\nExpression for the expected value: "<<#expected<<"\n";Q_UNREACHABLE();}
#define EXPECT_NEAR(value, expected) EXPECT_NEAR_EPSILON(value, expected, 0.0001f)
#define EXPECT_EQ(value, expected) if(value!=expected){qCritical() << "Expected"<<expected<<"got"<<value<<"\nExpression for the tested value: " <<#value<<"\nExpression for the expected value: "<<#expected<<"\n";Q_UNREACHABLE();}
#define EXPECT_TRUE(value) EXPECT_EQ(value, true);
#define EXPECT_FALSE(value) EXPECT_EQ(value, false);

glm::vec3 all_dimensions[6];

glrt::scene::CoordFrame a(glm::vec3(1, 2, 3),
                          glm::angleAxis(glm::radians(-90.f), glm::vec3(0, 0, 1)),
                          0.5f);
glrt::scene::CoordFrame b(glm::vec3(7, 8, 9),
                          glm::angleAxis(glm::radians(90.f), glm::vec3(1, 0, 0)),
                          3.f);

glm::mat4 ma = glm::mat4(0, -0.5f, 0, 0,
                         0.5f,  0, 0, 0,
                         0,  0, 0.5f, 0,
                         1,  2, 3, 1);

void test_transformations()
{
  EXPECT_NEAR(a* glm::vec3(0), glm::vec3(1, 2, 3));
  EXPECT_NEAR(b* glm::vec3(0), glm::vec3(7, 8, 9));

  EXPECT_NEAR(a* glm::vec3(0, 0, 1), glm::vec3(1, 2, 3.5));
  EXPECT_NEAR(b* glm::vec3(0, 0, 1), glm::vec3(7, 5, 9));
}

void test_concatenation()
{
  EXPECT_NEAR((a*b).scaleFactor, 1.5f);
  EXPECT_NEAR((a*b).position, a*glm::vec3(7, 8, 9));
  EXPECT_NEAR((a*b).orientation.x, (a.orientation*b.orientation).x);
  EXPECT_NEAR((a*b).orientation.y, (a.orientation*b.orientation).y);
  EXPECT_NEAR((a*b).orientation.z, (a.orientation*b.orientation).z);
  EXPECT_NEAR((a*b).orientation.w, (a.orientation*b.orientation).w);

  EXPECT_NEAR((a*b) * glm::vec3(0), glm::vec3(1, 2, 3) + glm::vec3(8, -7, 9)*.5f);
  EXPECT_NEAR((a*b) * glm::vec3(0, 0, 1), glm::vec3(1, 2, 3) + glm::vec3(5, -7, 9)*.5f);

  for(int i=0; i<6; ++i)
    EXPECT_NEAR((a*b)*all_dimensions[i], a*(b*all_dimensions[i]));
}

void test_to_matrix()
{
  EXPECT_NEAR(ma[0], a.toMat4()[0]);
  EXPECT_NEAR(ma[1], a.toMat4()[1]);
  EXPECT_NEAR(ma[2], a.toMat4()[2]);
  EXPECT_NEAR(ma[3], a.toMat4()[3]);
}

void test_from_matrix()
{
  EXPECT_NEAR(glrt::scene::CoordFrame(ma), a);
}

void test_invert()
{
  EXPECT_NEAR(a*a.inverse(), glrt::scene::CoordFrame());
  EXPECT_NEAR(a.inverse()*a, glrt::scene::CoordFrame());
  EXPECT_NEAR(b*b.inverse(), glrt::scene::CoordFrame());
  EXPECT_NEAR(b.inverse()*b, glrt::scene::CoordFrame());
}


int main(int argc, char** argv)
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  for(int i=0; i<6; ++i)
    all_dimensions[i][i/2] = (i%2==0 ? 1 : -1);

  test_transformations();
  test_concatenation();
  test_to_matrix();
  test_from_matrix();
  test_invert();

  return 0;
}
