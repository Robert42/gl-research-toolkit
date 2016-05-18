#ifndef _GLRT_GLSL_MATH_GLSL_H_
#define _GLRT_GLSL_MATH_GLSL_H_

#ifdef __GNUC__
#error Please, never include the math-glsl.h header from c++ code!
#endif

#define GLSL

#ifndef inline
#define inline
#endif

#define not_ not

#ifndef out
#define out(t) out t
#endif

#ifndef inout
#define inout(t) inout t
#endif

#include <openvdb/triangle-distance.glsl>
#include "math-common.h"
#include <debugging/print.glsl>

#define not_ not

#ifdef out
#undef out
#endif

#ifdef inout
#undef inout
#endif

#endif
