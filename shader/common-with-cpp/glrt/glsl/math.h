#ifndef _GLRT_GLSL_MATH_H_
#define _GLRT_GLSL_MATH_H_

#include <glrt/glsl/compatibility/begin.h>

const float pi = 3.1415926535897;

const float inf = 1./0.;

inline float sq(float x)
{
  return x*x;
}

inline float sq(vec2 x)
{
  return dot(x, x);
}

inline float sq(vec3 x)
{
  return dot(x, x);
}

inline float sq(vec4 x)
{
  return dot(x, x);
}

// min

inline int min3(int a, int b, int c)
{
  return min(min(a, b), c);
}

inline int min4(int a, int b, int c, int d)
{
  return min(min(a, b), min(c, d));
}

inline float min3(float a, float b, float c)
{
  return min(min(a, b), c);
}

inline float min4(float a, float b, float c, float d)
{
  return min(min(a, b), min(c, d));
}

inline vec4 min3(vec4 a, vec4 b, vec4 c)
{
  return min(min(a, b), c);
}

inline vec4 min4(vec4 a, vec4 b, vec4 c, vec4 d)
{
  return min(min(a, b), min(c, d));
}

inline float min_component(vec2 vector)
{
  return min(vector.x, vector.y);
}

inline float min_component(vec3 vector)
{
  return min3(vector.x, vector.y, vector.z);
}

inline float min_component(vec4 vector)
{
  return min4(vector.x, vector.y, vector.z, vector.w);
}

inline int index_of_first_true(bvec3 boolean)
{
  boolean.yz = boolean.yz && not_(boolean.xx);
  boolean.z = boolean.z && !boolean.y;

  return int(dot(vec2(1,2), vec2(boolean.yz)));
}

inline int index_of_first_true(bvec4 boolean)
{
  boolean.yzw = boolean.yzw && not_(boolean.xxx);
  boolean.zw = boolean.zw && not_(boolean.yy);
  boolean.w = boolean.w && !boolean.z;

  return int(dot(vec3(1,2,3), vec3(boolean.yzw)));
}

inline bvec3 is_smallest(vec3 vector)
{
  return lessThanEqual(vector.xyz, min(vector.yzx, vector.zxy));
}

inline bvec4 is_smallest(vec4 vector)
{
  return lessThanEqual(vector.xyzw, min3(vector.yzwx, vector.zwxy, vector.wxyz));
}

// max

inline int max3(int a, int b, int c)
{
  return max(max(a, b), c);
}

inline int max4(int a, int b, int c, int d)
{
  return max(max(a, b), max(c, d));
}

inline float max3(float a, float b, float c)
{
  return max(max(a, b), c);
}

inline float max4(float a, float b, float c, float d)
{
  return max(max(a, b), max(c, d));
}

inline vec4 max3(vec4 a, vec4 b, vec4 c)
{
  return max(max(a, b), c);
}

inline vec4 max4(vec4 a, vec4 b, vec4 c, vec4 d)
{
  return max(max(a, b), max(c, d));
}

inline float max_component(vec2 vector)
{
  return max(vector.x, vector.y);
}

inline float max_component(vec3 vector)
{
  return max3(vector.x, vector.y, vector.z);
}

inline float max_component(vec4 vector)
{
  return max4(vector.x, vector.y, vector.z, vector.w);
}



inline int index_of_min_component(vec3 vector)
{
  return index_of_first_true(is_smallest(vector));
}

inline int index_of_min_component_masked(vec3 vector, bvec3 mask)
{
  // preventing  the components, where mask is false to be the smallest one
  vector += 10.f*max_component(abs(vector)) * vec3(not_(mask));
  
  return index_of_first_true(is_smallest(vector));
}

inline int index_of_min_component(vec4 vector)
{
  return index_of_first_true(is_smallest(vector));
}

inline int index_of_min_component_masked(vec4 vector, bvec4 mask)
{
  // preventing  the components, where mask is false to be the smallest one
  vector += 10.f*max_component(abs(vector)) * vec4(not_(mask));
  
  return index_of_first_true(is_smallest(vector) && mask);
}

// blending

inline float blend_screen(float x,float y)
{
  return 1.f-(1.f-x)*(1.f-y);
}

inline vec2 blend_screen(vec2 x, vec2 y)
{
  return vec2(1)-(vec2(1)-x)*(vec2(1)-y);
}

inline vec3 blend_screen(vec3 x, vec3 y)
{
  return vec3(1)-(vec3(1)-x)*(vec3(1)-y);
}

inline vec4 blend_screen(vec4 x, vec4 y)
{
  return vec4(1)-(vec4(1)-x)*(vec4(1)-y);
}


#include <glrt/glsl/geometry/raytracing.h>
#include <glrt/glsl/geometry/transform.h>


#include <glrt/glsl/compatibility/end.h>

#endif // #define _GLRT_GLSL_MATH_H_
