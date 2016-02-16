// This header adds the possibility of comparing vectors easily

// This header is partly originally based on the gtest framework by Google.
// This is the original copyright notice:

// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: wan@google.com (Zhanyong Wan)
//

#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glrt/scene/coord-frame.h>

namespace testing {
namespace internal {

// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
template<typename T>
T sq(T x){return x*x;}

// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
template <typename VectorType>
struct VectorTraits;

// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
template <>
struct VectorTraits<glrt::scene::CoordFrame>
{
  typedef float RawType;
  typedef glrt::scene::CoordFrame VectorType;
  typedef int IteratorType;

  static const int number_floats = 8;

  static_assert(sizeof(VectorType) == number_floats*sizeof(float), "If CoordFrame has changed meanwhile");

  static int begin(const VectorType&){return 0;}
  static int end(const VectorType&){return number_floats;}
  static bool almostEqual(const VectorType& expectedCoord, const VectorType& actualCoord, int i_expected, int i_actual)
  {
    const float* expected = reinterpret_cast<const float*>(&expectedCoord);
    const float* actual = reinterpret_cast<const float*>(&actualCoord);
    const FloatingPoint<RawType> lhs(expected[i_expected]), rhs(actual[i_actual]);
    return lhs.AlmostEquals(rhs);
  }
  static double distance(const VectorType& expectedCoord, const VectorType& actualCoord)
  {
    double distance = 0.;
    const float* expected = reinterpret_cast<const float*>(&expectedCoord);
    const float* actual = reinterpret_cast<const float*>(&actualCoord);

    for(int i=0; i<number_floats; ++i)
      distance += sq(double(expected[i]) - double(actual[i]));

    return distance;
  }
};

// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
template <typename T, glm::precision p>
struct VectorTraits<glm::tvec2<T, p>>
{
  typedef T RawType;
  typedef glm::tvec2<T, p> VectorType;
  typedef int IteratorType;

  static int begin(const VectorType&){return 0;}
  static int end(const VectorType&){return 2;}
  static bool almostEqual(const VectorType& expected, const VectorType& actual, int i_expected, int i_actual)
  {
    const FloatingPoint<RawType> lhs(expected[i_expected]), rhs(actual[i_actual]);
    return lhs.AlmostEquals(rhs);
  }
  static double distance(const VectorType& a, const VectorType& b)
  {
    return glm::distance(a, b);
  }
};

// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
template <typename T, glm::precision p>
struct VectorTraits<glm::tvec3<T, p>>
{
  typedef T RawType;
  typedef glm::tvec3<T, p> VectorType;
  typedef int IteratorType;

  static int begin(const VectorType&){return 0;}
  static int end(const VectorType&){return 3;}
  static bool almostEqual(const VectorType& expected, const VectorType& actual, int i_expected, int i_actual)
  {
    const FloatingPoint<RawType> lhs(expected[i_expected]), rhs(actual[i_actual]);
    return lhs.AlmostEquals(rhs);
  }
  static double distance(const VectorType& a, const VectorType& b)
  {
    return glm::distance(a, b);
  }
};

// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
template <typename T, glm::precision p>
struct VectorTraits<glm::tvec4<T, p>>
{
  typedef T RawType;
  typedef glm::tvec4<T, p> VectorType;
  typedef int IteratorType;

  static int begin(const VectorType&){return 0;}
  static int end(const VectorType&){return 4;}
  static bool almostEqual(const VectorType& expected, const VectorType& actual, int i_expected, int i_actual)
  {
    const FloatingPoint<RawType> lhs(expected[i_expected]), rhs(actual[i_actual]);
    return lhs.AlmostEquals(rhs);
  }
  static double distance(const VectorType& a, const VectorType& b)
  {
    return glm::distance(a, b);
  }
};

// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
template <typename T, glm::precision p>
struct VectorTraits<glm::tquat<T, p>>
{
  typedef T RawType;
  typedef glm::tquat<T, p> VectorType;
  typedef int IteratorType;

  static int begin(const VectorType&){return 0;}
  static int end(const VectorType&){return 4;}
  static bool almostEqual(const VectorType& expected, const VectorType& actual, int i_expected, int i_actual)
  {
    const FloatingPoint<RawType> lhs(expected[i_expected]), rhs(actual[i_actual]);
    return lhs.AlmostEquals(rhs);
  }
  static double distance(const VectorType& a, const VectorType& b)
  {
    return glm::distance(a, b);
  }
};

// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
template<typename IteratorType>
bool compareWithIterator(IteratorType begin_expected, IteratorType begin_actual, const IteratorType& end_expected, const IteratorType& end_actual, const std::function<bool(const IteratorType&a, const IteratorType&b)>& isSame)
{
  while(begin_expected != end_expected)
  {
    if(begin_expected == end_actual)
      return false;

    if(!isSame(begin_expected, begin_expected))
      return false;

    ++begin_expected;
    ++begin_actual;
  }

  return begin_expected == end_actual;
}

// Helper template function for comparing floating-points.
//
// Template parameter:
//
//   VectorType: the raw vector type
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
template<typename VectorType>
AssertionResult CmpHelperVectorEQ(const char* expected_expression,
                                  const char* actual_expression,
                                  const VectorType& expected,
                                  const VectorType& actual)
{
  typedef VectorTraits<VectorType> Traits;
  typedef typename Traits::RawType RawType;
  typedef typename Traits::IteratorType IteratorType;

  if (compareWithIterator<IteratorType>(Traits::begin(expected),
                                        Traits::begin(actual),
                                        Traits::end(expected),
                                        Traits::end(actual),
                                        [&expected,&actual](const IteratorType& a, const IteratorType& b){return Traits::almostEqual(expected, actual, a, b);})) {
    return AssertionSuccess();
  }

  ::std::stringstream expected_ss;
  expected_ss << std::setprecision(std::numeric_limits<RawType>::digits10 + 2)
              << expected;

  ::std::stringstream actual_ss;
  actual_ss << std::setprecision(std::numeric_limits<RawType>::digits10 + 2)
            << actual;

  return EqFailure(expected_expression,
                   actual_expression,
                   StringStreamToString(&expected_ss),
                   StringStreamToString(&actual_ss),
                   false);
}

// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
template <typename VectorType>
AssertionResult CmpHelperVectorNear(const char* expected_expression,
                                    const char* actual_expression,
                                    const char* epsilon_expression,
                                    const VectorType& expected,
                                    const VectorType& actual,
                                    double epsilon)
{
  typedef VectorTraits<VectorType> Traits;

  double difference = Traits::distance(expected, actual);

  if(difference <= epsilon)
    return AssertionSuccess();

  return AssertionFailure()
      << "The difference between " << expected_expression << " and " << actual_expression
      << " is " << difference << ", which exceeds " << epsilon << ", where\n"
      << expected_expression << " evaluates to " << expected << ",\n"
      << actual_expression << " evaluates to " << actual << ", and\n"
      << epsilon_expression << " evaluates to " << epsilon << ".";
}



} // namespace internal
} // namespace testing

#define ASSERT_VEC_EQ(expected, actual)\
  ASSERT_PRED_FORMAT2(::testing::internal::CmpHelperVectorEQ, \
                      expected, actual)

#define EXPECT_VEC_EQ(expected, actual)\
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperVectorEQ, \
                      expected, actual)

#define ASSERT_VEC_NEAR(expected, actual, epsilon)\
  ASSERT_PRED_FORMAT3(::testing::internal::CmpHelperVectorNear, \
                      expected, actual, epsilon)

#define EXPECT_VEC_NEAR(expected, actual, epsilon)\
  EXPECT_PRED_FORMAT3(::testing::internal::CmpHelperVectorNear, \
                      expected, actual, epsilon)
