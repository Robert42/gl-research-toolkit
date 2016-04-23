// ======== C++ ================================================================
#if defined(__GNUC__)

namespace glrt {
namespace glsl {

using glm::bvec2;
using glm::bvec3;
using glm::bvec4;

using glm::vec2;
using glm::vec3;
using glm::vec4;


using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

using glm::mat3;
using glm::mat4;

using glm::dot;
using glm::max;
using glm::min;
using glm::clamp;
using glm::cos;
using glm::sin;
using glm::acos;
using glm::asin;

using glm::all;
using glm::any;

using glm::lessThanEqual;
using glm::lessThan;
using glm::greaterThanEqual;
using glm::greaterThan;

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
#define xz xz()
#define yw yw()
#define yz yz()
#define xy xy()
#define zw zw()

// ======== GLSL ===============================================================
#else

#ifndef inline
#define inline
#endif

#ifndef out
#define out(t) out t
#endif

#ifndef inout
#define inout(t) inout t
#endif

#include <debugging/print.glsl>

#endif
