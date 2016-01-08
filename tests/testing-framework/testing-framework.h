#ifndef TESTING_FRAMEWORK_H_
#define TESTING_FRAMEWORK_H_

#include <glrt/dependencies.h>

#include "testing-application.h"

#define EXPECT_NEAR_EPSILON(value, expected, epsilon) if(glm::distance(value, expected) > epsilon){qCritical() << "Expected"<<expected<<"got"<<value<<"\nExpression for the tested value: " <<#value<<"\nExpression for the expected value: "<<#expected<<"\n";Q_UNREACHABLE();}
#define EXPECT_NEAR(value, expected) EXPECT_NEAR_EPSILON(value, expected, 0.0001f)
#define EXPECT_EQ(value, expected) if(value!=expected){qCritical() << "Expected"<<expected<<"got"<<value<<"\nExpression for the tested value: " <<#value<<"\nExpression for the expected value: "<<#expected<<"\n";Q_UNREACHABLE();}
#define EXPECT_TRUE(value) EXPECT_EQ(value, true);
#define EXPECT_FALSE(value) EXPECT_EQ(value, false);

#endif // TESTING_FRAMEWORK_H_
