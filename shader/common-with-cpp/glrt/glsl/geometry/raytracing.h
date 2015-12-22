#ifndef _GLRT_GLSL_MATH_H_
#error Do not include this file directly, instead, include <glrt/glsl/math.h>
#endif


#include "./structs.h"

// ======== Ray ================================================================

Ray ray_from_two_points(vec3 from, vec3 to)
{
  Ray r;
  r.direction = normalize(to-from);
  r.origin = from;
  return r;
}

vec3 get_point(in Ray ray, float t)
{
  return ray.origin + ray.direction * t;
}

// ---- nearest_point

vec3 nearest_point(in Ray ray, in vec3 point)
{
  return ray.origin + ray.direction * max(0.f, dot(ray.direction, point-ray.origin));
}

vec3 nearest_point_unclamped(in Ray ray, in vec3 point)
{
  return ray.origin + ray.direction * dot(ray.direction, point-ray.origin);
}

vec3 nearest_point_clamped(in Ray ray, in vec3 point, float min, float max)
{
  return ray.origin + ray.direction * clamp(dot(ray.direction, point-ray.origin), min, max);
}

vec3 nearest_point_to_line_segment(vec3 from, vec3 to, in vec3 point)
{
  Ray r;
  r.direction = to-from;
  r.origin = from;
  float len = length(r.direction);
  r.direction /= len;
  
  return nearest_point_clamped(r, point, 0.f, len);
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

Plane plane_from_rect(in Rect rect)
{
  return plane_from_tangents(rect.tangent1, rect.tangent2, rect.origin);
}

Plane plane_for_projection(in Ray ray, float image_plane_distance)
{
  return plane_from_normal(ray.direction, get_point(ray, image_plane_distance));
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

// ---- front/backside

bool is_on_frontside(in Plane plane, in vec3 point)
{
  return signed_distance_to(plane, point) > 0;
}

// ---- nearest point

vec3 nearest_point(in Plane plane, in vec3 point)
{
  return point - plane.normal * signed_distance_to(plane, point);
}

// ---- contains

bool contains(in Plane plane, in vec3 point, float epsilon)
{
  float signed_distance = signed_distance_to(plane, point);

  return abs(signed_distance) < epsilon;
}

// ---- intersection


// returns infinity, if there's no intersection at all
float intersection_distance(in Plane plane, in Ray ray)
{
  // https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
  return (plane.d - dot(ray.origin, plane.normal)) / dot(ray.direction, plane.normal);
}

bool intersection_test_unclamped(in Plane plane, in Ray ray, out(float) distance)
{
  distance = intersection_distance(plane, ray);
  
  return !isnan(distance);
}

bool intersection_test(in Plane plane, in Ray ray, out(float) distance)
{
  distance = intersection_distance(plane, ray);
  
  return distance >= 0 && !isnan(distance);
}

bool intersection_point_unclamped(in Plane plane, in Ray ray, out(vec3) point)
{
  float t;
  bool intersects = intersection_test_unclamped(plane, ray, t);
  
  point = get_point(ray, t);
  
  return intersects;
}

bool intersection_point(in Plane plane, in Ray ray, out(vec3) point)
{
  float t;
  bool intersects = intersection_test(plane, ray, t);

  point = get_point(ray, t);

  return intersects;
}

// TODO: what if point is equal wo view_position?
vec3 perspective_projection_unclamped(in Plane plane, in vec3 view_position, in vec3 point)
{
  Ray ray;
  ray.direction = normalize(point-view_position);
  ray.origin = view_position;
      
  vec3 intersection_point;
  intersection_point_unclamped(plane, ray, intersection_point);
  return intersection_point;
}


// ======== Sphere =============================================================

bool intersects(in Sphere sphere, in Ray ray)
{
  float d = sq_distance_to(ray, sphere.origin);
  
  return d <= sq(sphere.radius);
}


// ======== Rect =============================================================

vec2 map_point_to_rect_plane(in Rect rect, in vec3 point)
{
  return vec2(dot(point-rect.origin, rect.tangent1),
              dot(point-rect.origin, rect.tangent2));
}

vec3 map_point_from_rect_plane(in Rect rect, in vec2 p)
{
  return rect.origin + p.x*rect.tangent1 + p.y*rect.tangent2;
}

vec2 clamp_point_to_rect(in vec2 half_size, in vec2 p)
{
  return clamp(p, -half_size, half_size);
}

vec2 clamp_point_to_rect(in Rect rect, in vec2 p)
{
  return clamp_point_to_rect(vec2(rect.half_width, rect.half_height), p);
}

vec3 clamp_point_to_rect(in Rect rect, in vec3 point)
{
  return map_point_from_rect_plane(rect, clamp_point_to_rect(rect, map_point_to_rect_plane(rect, point)));
}

bool contains_mapped_point(in Rect rect, in vec2 p)
{
  return p.x >= -rect.half_width && p.y >= -rect.half_height && p.x <= rect.half_width && p.y <= rect.half_height;
}

bool contains_mapped_point(in Rect rect, in vec3 p)
{
  return contains_mapped_point(rect, map_point_to_rect_plane(rect, p));
}

bool intersects_unclamped(in Rect rect, in Ray ray)
{
  Plane plane = plane_from_rect(rect);
  
  vec3 p;
  return intersection_point_unclamped(plane, ray, p) && contains_mapped_point(rect, p);
}

