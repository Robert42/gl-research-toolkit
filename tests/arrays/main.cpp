#include <testing-framework.h>

#include <testing-framework.h>

#include "statespy.h"

using glrt::Array;



void test_capacity_traits();

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
  StateSpy::clearIndex();
  StateSpy::clearLog();

  StateSpyArray array1, array2;

  EXPECT_TRUE(StateSpy::log().isEmpty());

  array1.append(StateSpy());
  array2.append(StateSpy());

  EXPECT_FALSE(StateSpy::log().isEmpty());
  StateSpy::clearLog();
  EXPECT_TRUE(StateSpy::log().isEmpty());

  array1.swap(array2);

  // No copying/moving/anything should happen during swapping
  EXPECT_TRUE(StateSpy::log().isEmpty());
}

void test_append_move()
{
  StateSpy::clearLog();

  {
    StateSpy::clearIndex();
    StateSpyArray array;
    array.append(StateSpy());

    EXPECT_EQ(StateSpy::log(),
              "0: default constructor\n"
              "1: move constructor from 0\n"
              "0: destructed\n");
    StateSpy::clearLog();

    EXPECT_EQ(array.capacity(), 2);
  }
  EXPECT_EQ(StateSpy::log(),
            "1: destructed\n");
  StateSpy::clearLog();

  {
    StateSpy::clearIndex();
    StateSpyArray array;

    array.append(StateSpy());
    EXPECT_EQ(StateSpy::log(),
              "0: default constructor\n"
              "1: move constructor from 0\n"
              "0: destructed\n");
    StateSpy::clearLog();

    array.append(StateSpy());
    EXPECT_EQ(StateSpy::log(),
              "2: default constructor\n"
              "3: move constructor from 2\n"
              "2: destructed\n");
    StateSpy::clearLog();

    EXPECT_EQ(array.capacity(), 2);
  }

  EXPECT_EQ(StateSpy::log(),
            "1: destructed\n"
            "3: destructed\n");
  StateSpy::clearLog();

  {
    StateSpy::clearIndex();
    StateSpyArray array;

    array.append(StateSpy());
    EXPECT_EQ(StateSpy::log(),
              "0: default constructor\n"
              "1: move constructor from 0\n"
              "0: destructed\n");
    StateSpy::clearLog();

    array.append(StateSpy());
    EXPECT_EQ(StateSpy::log(),
              "2: default constructor\n"
              "3: move constructor from 2\n"
              "2: destructed\n");
    StateSpy::clearLog();

    array.append(StateSpy());
    EXPECT_EQ(array.capacity(), 4);
    EXPECT_EQ(StateSpy::log(),
              "4: default constructor\n"
              "5: move constructor from 1\n"
              "6: move constructor from 3\n"
              "1: destructed\n"
              "3: destructed\n"
              "7: move constructor from 4\n"
              "4: destructed\n");
    StateSpy::clearLog();
  }

  EXPECT_EQ(StateSpy::log(),
            "5: destructed\n"
            "6: destructed\n"
            "7: destructed\n");
  StateSpy::clearLog();

}

void test_append_copy()
{
  StateSpy::clearLog();

  {
    StateSpy::clearIndex();
    StateSpyArray array;
    {
      StateSpy s;
      array.append(s);
    }

    EXPECT_EQ(StateSpy::log(),
              "0: default constructor\n"
              "1: copy constructor from 0\n"
              "0: destructed\n");
    StateSpy::clearLog();

    EXPECT_EQ(array.capacity(), 2);
  }
  EXPECT_EQ(StateSpy::log(),
            "1: destructed\n");
  StateSpy::clearLog();

  {
    StateSpy::clearIndex();
    StateSpyArray array;

    {
      StateSpy s;
      array.append(s);
    }
    EXPECT_EQ(StateSpy::log(),
              "0: default constructor\n"
              "1: copy constructor from 0\n"
              "0: destructed\n");
    StateSpy::clearLog();

    {
      StateSpy s;
      array.append(s);
    }
    EXPECT_EQ(StateSpy::log(),
              "2: default constructor\n"
              "3: copy constructor from 2\n"
              "2: destructed\n");
    StateSpy::clearLog();

    EXPECT_EQ(array.capacity(), 2);
  }

  EXPECT_EQ(StateSpy::log(),
            "1: destructed\n"
            "3: destructed\n");
  StateSpy::clearLog();

  {
    StateSpy::clearIndex();
    StateSpyArray array;

    {
      StateSpy s;
      array.append(s);
    }
    EXPECT_EQ(StateSpy::log(),
              "0: default constructor\n"
              "1: copy constructor from 0\n"
              "0: destructed\n");
    StateSpy::clearLog();

    {
      StateSpy s;
      array.append(s);
    }
    EXPECT_EQ(StateSpy::log(),
              "2: default constructor\n"
              "3: copy constructor from 2\n"
              "2: destructed\n");
    StateSpy::clearLog();

    {
      StateSpy s;
      array.append(s);
    }
    EXPECT_EQ(array.capacity(), 4);
    EXPECT_EQ(StateSpy::log(),
              "4: default constructor\n"
              "5: move constructor from 1\n"
              "6: move constructor from 3\n"
              "1: destructed\n"
              "3: destructed\n"
              "7: copy constructor from 4\n"
              "4: destructed\n");
    StateSpy::clearLog();
  }

  EXPECT_EQ(StateSpy::log(),
            "5: destructed\n"
            "6: destructed\n"
            "7: destructed\n");
  StateSpy::clearLog();

}

void test_extend_copy()
{
  StateSpy::clearLog();

  {
    StateSpy::clearIndex();
    StateSpyArray array;

    {

      StateSpy ab[2];

      EXPECT_EQ(StateSpy::log(),
                "0: default constructor\n"
                "1: default constructor\n");
      StateSpy::clearLog();

      array.extend_copy(ab, 2);
      EXPECT_EQ(array.capacity(), 2);

      EXPECT_EQ(StateSpy::log(),
                "2: copy constructor from 0\n"
                "3: copy constructor from 1\n");
      StateSpy::clearLog();

    }

    EXPECT_EQ(StateSpy::log(),
              "1: destructed\n"
              "0: destructed\n");
    StateSpy::clearLog();

  }

  EXPECT_EQ(StateSpy::log(),
            "2: destructed\n"
            "3: destructed\n");
  StateSpy::clearLog();


  {
    StateSpy::clearIndex();
    StateSpyArray array;

    {
      StateSpy abc[3];

      EXPECT_EQ(StateSpy::log(),
                "0: default constructor\n"
                "1: default constructor\n"
                "2: default constructor\n");
      StateSpy::clearLog();

      array.extend_copy(abc, 1);
      EXPECT_EQ(array.capacity(), 2);

      EXPECT_EQ(StateSpy::log(),
                "3: copy constructor from 0\n");
      StateSpy::clearLog();

      array.extend_copy(abc+1, 2);
      EXPECT_EQ(array.capacity(), 4);

      EXPECT_EQ(StateSpy::log(),
                "4: move constructor from 3\n"
                "3: destructed\n"
                "5: copy constructor from 1\n"
                "6: copy constructor from 2\n");
      StateSpy::clearLog();

    }

    EXPECT_EQ(StateSpy::log(),
              "2: destructed\n"
              "1: destructed\n"
              "0: destructed\n");
    StateSpy::clearLog();

  }

  EXPECT_EQ(StateSpy::log(),
            "4: destructed\n"
            "5: destructed\n"
            "6: destructed\n");
  StateSpy::clearLog();
}

void test_extend_move()
{
  StateSpy::clearLog();

  {
    StateSpy::clearIndex();
    StateSpyArray array;

    {

      StateSpy ab[2];

      EXPECT_EQ(StateSpy::log(),
                "0: default constructor\n"
                "1: default constructor\n");
      StateSpy::clearLog();

      array.extend_move(ab, 2);
      EXPECT_EQ(array.capacity(), 2);

      EXPECT_EQ(StateSpy::log(),
                "2: move constructor from 0\n"
                "3: move constructor from 1\n");
      StateSpy::clearLog();

    }

    EXPECT_EQ(StateSpy::log(),
              "1: destructed\n"
              "0: destructed\n");
    StateSpy::clearLog();

  }

  EXPECT_EQ(StateSpy::log(),
            "2: destructed\n"
            "3: destructed\n");
  StateSpy::clearLog();


  {
    StateSpy::clearIndex();
    StateSpyArray array;

    {
      StateSpy abc[3];

      EXPECT_EQ(StateSpy::log(),
                "0: default constructor\n"
                "1: default constructor\n"
                "2: default constructor\n");
      StateSpy::clearLog();

      array.extend_move(abc, 1);
      EXPECT_EQ(array.capacity(), 2);

      EXPECT_EQ(StateSpy::log(),
                "3: move constructor from 0\n");
      StateSpy::clearLog();

      array.extend_move(abc+1, 2);
      EXPECT_EQ(array.capacity(), 4);

      EXPECT_EQ(StateSpy::log(),
                "4: move constructor from 3\n"
                "3: destructed\n"
                "5: move constructor from 1\n"
                "6: move constructor from 2\n");
      StateSpy::clearLog();

    }

    EXPECT_EQ(StateSpy::log(),
              "2: destructed\n"
              "1: destructed\n"
              "0: destructed\n");
    StateSpy::clearLog();

  }

  EXPECT_EQ(StateSpy::log(),
            "4: destructed\n"
            "5: destructed\n"
            "6: destructed\n");
  StateSpy::clearLog();
}

void test_destructor()
{
  StateSpy::clearLog();
  {
    StateSpy::clearIndex();
    StateSpyArray array;

    EXPECT_TRUE(array.isEmpty());
  }
  EXPECT_EQ(StateSpy::log(),
            "");
  StateSpy::clearLog();

  {
    StateSpy::clearIndex();
    StateSpyArray array;

    array.append(StateSpy());

    EXPECT_FALSE(array.isEmpty());
  }
  EXPECT_EQ(StateSpy::log(),
            "0: default constructor\n"
            "1: move constructor from 0\n"
            "0: destructed\n"
            "1: destructed\n");
  StateSpy::clearLog();


  {
    StateSpy::clearIndex();
    StateSpyArray array = {StateSpy()};
    EXPECT_EQ(StateSpy::log(),
              "0: default constructor\n"
              "1: copy constructor from 0\n"
              "0: destructed\n");
    StateSpy::clearLog();
    EXPECT_FALSE(array.isEmpty());
  }
  EXPECT_EQ(StateSpy::log(),
            "1: destructed\n");
  StateSpy::clearLog();
}

void test_remove()
{
  StateSpy::clearLog();

  // test removing single
  {
    StateSpy::clearIndex();
    StateSpyArray array = {StateSpy()};
    EXPECT_EQ(StateSpy::log(),
              "0: default constructor\n"
              "1: copy constructor from 0\n"
              "0: destructed\n");
    StateSpy::clearLog();
    EXPECT_FALSE(array.isEmpty());
    EXPECT_EQ(array.capacity(), 2);
    array.remove(0);
    EXPECT_TRUE(array.isEmpty());
    EXPECT_EQ(array.capacity(), 0);
    EXPECT_EQ(StateSpy::log(),
              "1: destructed\n");
    StateSpy::clearLog();
  }
  EXPECT_EQ(StateSpy::log(),
            "");
  StateSpy::clearLog();

  {
    StateSpy::clearIndex();
    StateSpyArray array = {StateSpy(), StateSpy()};
    EXPECT_EQ(StateSpy::log(),
              "0: default constructor\n"
              "1: default constructor\n"
              "2: copy constructor from 0\n"
              "3: copy constructor from 1\n"
              "1: destructed\n"
              "0: destructed\n");
    StateSpy::clearLog();
    EXPECT_EQ(array.length(), 2);
    EXPECT_EQ(array.capacity(), 2);
    array.remove(0);
    EXPECT_EQ(array.length(), 1);
    EXPECT_EQ(array.capacity(), 2);
    EXPECT_EQ(StateSpy::log(),
              "2: move operator from 3\n"
              "3: destructed\n");
    StateSpy::clearLog();
    array.remove(0);
    EXPECT_EQ(array.length(), 0);
    EXPECT_EQ(array.capacity(), 0);
    EXPECT_EQ(StateSpy::log(),
              "2: destructed\n");
    StateSpy::clearLog();
  }
  EXPECT_EQ(StateSpy::log(),
            "");
  StateSpy::clearLog();


  // test removing single changing internal buffer
  {
    StateSpy::clearIndex();
    StateSpyArray_CapacityBock_2_2 array = {StateSpy(), StateSpy(), StateSpy()};
    EXPECT_EQ(StateSpy::log(),
              "0: default constructor\n"
              "1: default constructor\n"
              "2: default constructor\n"
              "3: copy constructor from 0\n"
              "4: copy constructor from 1\n"
              "5: copy constructor from 2\n"
              "2: destructed\n"
              "1: destructed\n"
              "0: destructed\n");
    StateSpy::clearLog();
    EXPECT_EQ(array.length(), 3);
    EXPECT_EQ(array.capacity(), 4);
    array.remove(1);
    EXPECT_EQ(array.length(), 2);
    EXPECT_EQ(array.capacity(), 2);
    EXPECT_EQ(StateSpy::log(),
              "4: move operator from 5\n"
              "5: destructed\n"
              "6: move constructor from 3\n"
              "7: move constructor from 4\n"
              "3: destructed\n"
              "4: destructed\n");
    StateSpy::clearLog();
    array.remove(1);
    EXPECT_EQ(array.length(), 1);
    EXPECT_EQ(array.capacity(), 2);
    EXPECT_EQ(StateSpy::log(),
              "7: destructed\n");
    StateSpy::clearLog();
    array.remove(0);
    EXPECT_EQ(array.length(), 0);
    EXPECT_EQ(array.capacity(), 0);
    EXPECT_EQ(array.data(), nullptr);
    EXPECT_EQ(StateSpy::log(),
              "6: destructed\n");
    StateSpy::clearLog();
  }
  EXPECT_EQ(StateSpy::log(),
            "");
  StateSpy::clearLog();

  // test removing multiple
  {
    StateSpy::clearIndex();
    StateSpyArray_CapacityBock_2_2 array = {StateSpy(), StateSpy(), StateSpy()};
    EXPECT_EQ(StateSpy::log(),
              "0: default constructor\n"
              "1: default constructor\n"
              "2: default constructor\n"
              "3: copy constructor from 0\n"
              "4: copy constructor from 1\n"
              "5: copy constructor from 2\n"
              "2: destructed\n"
              "1: destructed\n"
              "0: destructed\n");
    StateSpy::clearLog();
    EXPECT_EQ(array.length(), 3);
    EXPECT_EQ(array.capacity(), 4);
    array.remove(0, 2);
    EXPECT_EQ(array.length(), 1);
    EXPECT_EQ(array.capacity(), 2);
    EXPECT_EQ(StateSpy::log(),
              "3: move operator from 5\n"
              "4: destructed\n"
              "5: destructed\n"
              "6: move constructor from 3\n"
              "3: destructed\n");
    StateSpy::clearLog();
    array.remove(0, 1);
    EXPECT_EQ(array.length(), 0);
    EXPECT_EQ(array.capacity(), 0);
    EXPECT_EQ(array.data(), nullptr);
    EXPECT_EQ(StateSpy::log(),
              "6: destructed\n");
    StateSpy::clearLog();
  }
  EXPECT_EQ(StateSpy::log(),
            "");
  StateSpy::clearLog();
}

void test_ensureCapacity()
{
  StateSpyArray array;
  EXPECT_EQ(array.capacity(), 0);

  array.ensureCapacity(0);

  EXPECT_EQ(array.capacity(), 0);

  array.ensureCapacity(1);

  EXPECT_EQ(array.capacity(), 2);

  array.ensureCapacity(2);

  EXPECT_EQ(array.capacity(), 2);

  array.ensureCapacity(0);

  EXPECT_EQ(array.capacity(), 2);

  array.ensureCapacity(3);

  EXPECT_EQ(array.capacity(), 4);

  array.ensureCapacity(1);

  EXPECT_EQ(array.capacity(), 4);
}

int main(int argc, char** argv)
{
  TestingApplication testing_application(argc, argv);

  test_capacity_traits();
  test_array_constructors();
  test_array_operators();
  test_swap();
  test_append_move();
  test_append_copy();
  test_extend_move();
  test_extend_copy();
  test_destructor();
  test_remove();
  test_ensureCapacity();

  // #TODO: Test all trait functions in action. Goal: each code line in the functions should be executed at least once.

  return testing_application.result();
}
