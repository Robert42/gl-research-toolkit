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

#include <glrt/glsl/compatibility/end.h>
