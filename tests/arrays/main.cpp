#include <testing-framework.h>
#include <glrt/toolkit/array.h>

#include <testing-framework.h>

#include "statespy.h"

using glrt::Array;



void test_unordered_array_trait();

void test_array_constructors()
{
  // Test standart constructor
  Array<int> array1;

  EXPECT_TRUE(array1.isEmpty());
  EXPECT_EQ(array1.length(), 0);
  EXPECT_EQ(array1.capacity(), 0);

  // Test constructor with initializer List
  Array<int> array2 = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109};

  EXPECT_EQ(array2.length(), 10);
  EXPECT_TRUE(array2.capacity()>=10); // when initializing an array with an initializer list, the capacity is set to the number of init values
  EXPECT_FALSE(array2.isEmpty());
  EXPECT_EQ(array2, Array<int>({100, 101, 102, 103, 104, 105, 106, 107, 108, 109}));


  // Test standart move constructor
  Array<int> array3(std::move(array2));

  EXPECT_TRUE(array2.isEmpty());
  EXPECT_FALSE(array3.isEmpty());
  EXPECT_EQ(array3, Array<int>({100, 101, 102, 103, 104, 105, 106, 107, 108, 109}));
}

void test_array_operators()
{
  // test == && !=
  Array<int> array1 = {0, 1, 2};
  Array<int> array2 = {0};
  Array<int> array3 = {0, 1, 2, 3};
  Array<int> array4 = {0, 1, 2, 3};
  Array<int> array5;
  EXPECT_EQ(array1, array1);
  EXPECT_NEQ(array1, array2);
  EXPECT_NEQ(array1, array3);
  EXPECT_NEQ(array1, array4);
  EXPECT_NEQ(array1, array5);
  EXPECT_NEQ(array2, array1);
  EXPECT_EQ (array2, array2);
  EXPECT_NEQ(array2, array3);
  EXPECT_NEQ(array2, array4);
  EXPECT_NEQ(array2, array5);
  EXPECT_NEQ(array3, array1);
  EXPECT_NEQ(array3, array2);
  EXPECT_EQ (array3, array3);
  EXPECT_EQ (array3, array4);
  EXPECT_NEQ(array3, array5);
  EXPECT_NEQ(array4, array1);
  EXPECT_NEQ(array4, array2);
  EXPECT_EQ (array4, array3);
  EXPECT_EQ (array4, array4);
  EXPECT_NEQ(array4, array5);
  EXPECT_NEQ(array5, array1);
  EXPECT_NEQ(array5, array2);
  EXPECT_NEQ(array5, array3);
  EXPECT_NEQ(array5, array4);
  EXPECT_EQ (array5, array5);

  // test move operator (is implemented using swap, so ne need to test swap seprately)
  array1 = std::move(array1);
  array5 = std::move(array5);
  EXPECT_EQ(array1, Array<int>({0, 1, 2}));
  EXPECT_TRUE(array5.isEmpty());

  array1 = std::move(array5);
  EXPECT_EQ(array5, Array<int>({0, 1, 2}));
  EXPECT_TRUE(array1.isEmpty());

  array1 = std::move(array5);
  EXPECT_EQ(array1, Array<int>({0, 1, 2}));
  EXPECT_TRUE(array5.isEmpty());

  // test [] (is using the at() function, so no need to test at() seperately)
  EXPECT_EQ(array1[0], 0);
  EXPECT_EQ(array1[1], 1);
  EXPECT_EQ(array1[2], 2);
}

void test_swap()
{
  StateSpy::clear();
  Array<StateSpy, glrt::ArrayTraits_Unordered_mCmOmID<StateSpy>> array1;
  Array<StateSpy, glrt::ArrayTraits_Unordered_mCmOmID<StateSpy>> array2;

  EXPECT_TRUE(StateSpy::log().isEmpty());

  array1.append(StateSpy());
  array2.append(StateSpy());

  EXPECT_FALSE(StateSpy::log().isEmpty());
  StateSpy::clear();
  EXPECT_TRUE(StateSpy::log().isEmpty());

  array1.swap(array2);

  // No copying/moving/anything should happen during swapping
  EXPECT_TRUE(StateSpy::log().isEmpty());
}

void test_clean()
{
  StateSpy::clear();
  StateSpy::enabledDebugPrint = true;
  Array<StateSpy, glrt::ArrayTraits_Unordered_mCmOmID<StateSpy>> array1;
  array1.append(StateSpy());
  array1.append(StateSpy());
  array1.append(StateSpy());
  StateSpy::enabledDebugPrint = false;
}


int main(int argc, char** argv)
{
  TestingApplication testing_application(argc, argv);

  test_array_constructors();
  test_array_operators();
  test_unordered_array_trait();
  test_swap();
  test_clean();

  return testing_application.result();
}
