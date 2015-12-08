#include <glrt/glsl/compatibility/begin.h>


// ======== Structs ==========================================================


struct Sphere
{
  vec3 origin;
  float radius;
};

// https://de.wikipedia.org/wiki/Hessesche_Normalform
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


// ======== Helper =============================================================


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


// ======== Ray ================================================================


// ---- nearest_point

vec3 nearest_point(in Ray ray, in vec3 point)
{
  return ray.origin + ray.direction * max(0.f, dot(ray.direction, point));
}

vec3 nearest_point_unclamped(in Ray ray, in vec3 point)
{
  return ray.origin + ray.direction * dot(ray.direction, point);
}

// ---- distance

float sq_distance_to_unclamped(in Ray ray, in vec3 point)
{
  return sq(nearest_point_unclamped(ray, point)-point);
}

float distance_to_unclamped(in Ray ray, in vec3 point)
{
  return distance(nearest_point_unclamped(ray, point), point);
}

float sq_distance_to(in Ray ray, in vec3 point)
{
  return sq(nearest_point(ray, point)-point);
}

float distance_to(in Ray ray, in vec3 point)
{
  return distance(nearest_point(ray, point), point);
}

// ---- contains

bool contains(in Ray ray, in vec3 point, float epsilon)
{
  return sq_distance_to(ray, point)<= sq(epsilon);
}

bool contains_unclamped(in Ray ray, in vec3 point, float epsilon)
{
  return sq_distance_to_unclamped(ray, point)<= sq(epsilon);
}


// ======== Plane ==============================================================

// ---- construct

// normal must have length 1
Plane plane_from_normal(in vec3 normal, in float d)
{
  Plane p;

  p.normal = normal;
  p.d = d;

  return p;
}

// normal must have length 1
Plane plane_from_normal(in vec3 normal, in vec3 point)
{
  return plane_from_normal(normal, dot(normal, point));
}

// normal will be normalized t1 x t2
Plane plane_from_tangents(in vec3 tangent1, in vec3 tangent2, in vec3 origin)
{
  return plane_from_normal(normalize(cross(tangent1, tangent2)),
                           origin);
}

// a,b,c ccw will have the normal point towards the viewer
Plane plane_from_three_points(in vec3 a, in vec3 b, in vec3 c)
{
  return plane_from_tangents(a-c, b-c, c);
}

// ---- distances

// https://de.wikipedia.org/wiki/Hessesche_Normalform#Abstand_2
float signed_distance_to(in Plane plane, in vec3 point)
{
  return dot(plane.normal, point) - plane.d;
}

float distance_to(in Plane plane, in vec3 point)
{
  return abs(signed_distance_to(plane, point));
}

// ---- nearest point

// TODO

// ---- contains

bool contains(in Plane plane, in vec3 point, float epsilon)
{
  float signed_distance = signed_distance_to(plane, point);

  return abs(signed_distance) < epsilon;
}

// ---- intersection

// TODO


// ======== Sphere =============================================================
/*
vec3 nearest_point_on_sphere(in Sphere sphere, in Plane ray)
{
  Plane 
}

vec3 nearest_point_on_sphere(in Sphere sphere, in Ray ray)
{
  return nearest_point_on_sphere(sphere,
                                 ); 
}
*/


#include <glrt/glsl/compatibility/end.h>
