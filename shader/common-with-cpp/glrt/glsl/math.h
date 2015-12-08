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

#include <glrt/glsl/compatibility/end.h>
