#include <glrt/glsl/compatibility/begin.h>

struct Sphere
{
  vec3 origin;
  float radius;
};

struct Plane
{
  vec3 normal;
  float d;
};

struct Ray
{
  vec3 origin;
  vec3 direction;
};



// ======== Distances ==========================================================

float signed_distance_to(in Plane plane, in vec3 point)
{
  return dot(plane.normal, point) - plane.d;
}

#include <glrt/glsl/compatibility/end.h>
