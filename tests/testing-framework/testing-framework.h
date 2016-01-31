#ifndef TESTING_FRAMEWORK_H_
#define TESTING_FRAMEWORK_H_

#include <glrt/dependencies.h>

#include "testing-application.h"

// #FIXME use gtest
#define EXPECT_NEAR_EPSILON(value, expected, epsilon) if(glm::distance(value, expected) > epsilon){qCritical() << "Expected"<<expected<<"got"<<value<<"\nExpression for the tested value: " <<#value<<"\nExpression for the expected value: "<<#expected<<"\n";Q_UNREACHABLE();}
#define EXPECT_NEAR(value, expected) EXPECT_NEAR_EPSILON(value, expected, 0.0001f)
#define EXPECT_EQ(value, expected) if((value)!=(expected)){qCritical() << "Expected"<<(expected)<<"got"<<(value)<<"\nExpression for the tested value: " <<#value<<"\nExpression for the expected value: "<<#expected<<"\n";Q_UNREACHABLE();}
#define EXPECT_NEQ(value, expected) if((value)==(expected)){qCritical() << "Expected something different to"<<(expected)<<"got"<<(value)<<"\nExpression for the tested value: " <<#value<<"\nExpression for the expected value: "<<#expected<<"\n";Q_UNREACHABLE();}
#define EXPECT_TRUE(value) EXPECT_EQ(value, true);
#define EXPECT_FALSE(value) EXPECT_EQ(value, false);

#define ASSERT_EQ(value, expected) EXPECT_EQ(value, expected)
#define ASSERT_TRUE(value) EXPECT_TRUE(value)
#define ASSERT_FALSE(value) EXPECT_FALSE(value)

#endif // TESTING_FRAMEWORK_H_
