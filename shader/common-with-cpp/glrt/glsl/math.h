#ifndef _GLRT_GLSL_MATH_H_
#define _GLRT_GLSL_MATH_H_

#include <glrt/glsl/compatibility/begin.h>

const float pi = 3.1415926535897;

const float inf = 1./0.;

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

// min

int min3(int a, int b, int c)
{
  return min(min(a, b), c);
}

int min4(int a, int b, int c, int d)
{
  return min(min(a, b), min(c, d));
}

float min3(float a, float b, float c)
{
  return min(min(a, b), c);
}

float min4(float a, float b, float c, float d)
{
  return min(min(a, b), min(c, d));
}

float min_component(vec2 vector)
{
  return min(vector.x, vector.y);
}

float min_component(vec3 vector)
{
  return min3(vector.x, vector.y, vector.z);
}

float min_component(vec4 vector)
{
  return min4(vector.x, vector.y, vector.z, vector.w);
}

// max

int max3(int a, int b, int c)
{
  return max(max(a, b), c);
}

int max4(int a, int b, int c, int d)
{
  return max(max(a, b), max(c, d));
}

float max3(float a, float b, float c)
{
  return max(max(a, b), c);
}

float max4(float a, float b, float c, float d)
{
  return max(max(a, b), max(c, d));
}

float max_component(vec2 vector)
{
  return max(vector.x, vector.y);
}

float max_component(vec3 vector)
{
  return max3(vector.x, vector.y, vector.z);
}

float max_component(vec4 vector)
{
  return max4(vector.x, vector.y, vector.z, vector.w);
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
