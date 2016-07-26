#ifndef _GLRT_GLSL_MATH_CPP_H_
#define _GLRT_GLSL_MATH_CPP_H_

#ifndef __GNUC__
#error Please, never include the math-cpp.h header from a shader!
#endif

#include <openvdb/triangle-distance.h>

namespace glm
{
  inline float step(float edge, float x)
  {
    return step(vec3(edge), vec3(x)).x;
  }
}

namespace glrt {
namespace glsl {

using openvdb::math::closestPointOnTriangleToPoint;

using glm::bvec2;
using glm::bvec3;
using glm::bvec4;

using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::uvec2;
using glm::uvec3;
using glm::uvec4;


using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

using glm::mat4x3;
using glm::mat3;
using glm::mat4;

using glm::sign;
using glm::dot;
using glm::max;
using glm::min;
using glm::clamp;
using glm::step;
using glm::mix;
using glm::cos;
using glm::sin;
using glm::acos;
using glm::asin;
using glm::abs;

using glm::all;
using glm::any;
using glm::not_;

using glm::lessThanEqual;
using glm::lessThan;
using glm::greaterThanEqual;
using glm::greaterThan;

using glm::isnan;

#ifndef in
#define in
#endif

#ifndef out
#define out(t) t&
#endif

#ifndef inout
#define inout(t) t&
#endif

#undef PRINT_VALUE

template<typename T>
inline void PRINT_VALUE(const T&, bool x=false){Q_UNUSED(x);}

#define xyz xyz()
#define xyxy xyxy()
#define xx xx()
#define xxx xxx()
#define xxxx xxxx()
#define yy yy()
#define yyy yyy()
#define yyyy yyyy()
#define zz zz()
#define zzz zzz()
#define zzzz zzzz()
#define ww ww()
#define www www()
#define wwww wwww()
#define yzw yzw()
#define yzx yzx()
#define zxy zxy()
#define xz xz()
#define yw yw()
#define yz yz()
#define xy xy()
#define zw zw()
#define xyzw xyzw()
#define yzwx yzwx()
#define zwxy zwxy()
#define wxyz wxyz()

#include "math-common.h"


#ifdef out
#undef out
#endif

#ifdef in
#undef in
#endif

#ifdef inout
#undef inout
#endif

#undef xyz
#undef xyxy
#undef xx
#undef xxx
#undef xxxx
#undef yy
#undef yyy
#undef yyyy
#undef yzw
#undef yzx
#undef zxy
#undef xz
#undef yw
#undef yz
#undef xy
#undef zw
#undef xyzw
#undef yzwx
#undef zwxy
#undef wxyz

} // namespace glsl
} // namespace glrt

#endif // _GLRT_GLSL_MATH_CPP_H_
