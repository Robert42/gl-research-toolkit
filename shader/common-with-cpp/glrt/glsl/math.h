#ifndef _GLRT_GLSL_MATH_H_
#define _GLRT_GLSL_MATH_H_

#include <glrt/glsl/compatibility/begin.h>

float pi = 3.1415926535897;

float sq(float x)
{
  return x*x;
}

float sq(vec2 x)
{
  return dot(x, x);
}

float sq(vec3 x)
{
  return dot(x, x);
}

float sq(vec4 x)
{
  return dot(x, x);
}

float min_component(vec2 vector)
{
  // TODO optimize by using bvectors?
  // TODO: also add versions for min vec3, max vec2 and max vec3
  if(vector.x < vector.y)
    return vector.x;
  else
    return vector.y;
}

// blending

float blend_screen(float x,float y)
{
  return 1.f-(1.f-x)*(1.f-y);
}

vec2 blend_screen(vec2 x, vec2 y)
{
  return vec2(1)-(vec2(1)-x)*(vec2(1)-y);
}

vec3 blend_screen(vec3 x, vec3 y)
{
  return vec3(1)-(vec3(1)-x)*(vec3(1)-y);
}

vec4 blend_screen(vec4 x, vec4 y)
{
  return vec4(1)-(vec4(1)-x)*(vec4(1)-y);
}


#include <glrt/glsl/geometry/raytracing.h>

#include <glrt/glsl/compatibility/end.h>

#endif // #define _GLRT_GLSL_MATH_H_
