// ======== C++ ================================================================
#if defined(__GNUC__)

#include <glrt/dependencies.h>

namespace glrt {
namespace glsl {

using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

#ifndef in
#define in
#endif

#ifndef out
#define out(t) t&
#endif

#ifndef inout
#define inout(t) t&
#endif

// ======== GLSL ===============================================================
#else

#ifndef out
#define out(t) out t
#endif

#ifndef inout
#define inout(t) inout t
#endif

#endif
