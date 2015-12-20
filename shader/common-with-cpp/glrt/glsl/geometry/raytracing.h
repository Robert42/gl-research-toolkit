#ifndef _GLRT_GLSL_MATH_H_
#error Do not include this file directly, instead, include <glrt/glsl/math.h>
#endif


#include "./structs.h"

// ======== Ray ================================================================

vec3 get_point(in Ray ray, float t)
{
  return ray.origin + ray.direction * t;
}

// ---- nearest_point

vec3 nearest_point(in Ray ray, in vec3 point)
{
  return ray.origin + ray.direction * max(0.f, dot(ray.direction, point));
}

vec3 nearest_point_unclamped(in Ray ray, in vec3 point)
{
  return ray.origin + ray.direction * dot(ray.direction, point-ray.origin);
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


vec3 _most_representative_point_on_plane(in Plane plane, in Ray ray)
{
  float t;
  bool intersects = intersection_test(plane, ray, t);
  
  if(intersects)
  {
    return get_point(ray, t);
  }else
  {
    ray.direction -= (dot(ray.direction, plane.normal)*2.f + 0.0001f) * plane.normal;
    ray.direction = normalize(ray.direction);
    
    t = intersection_distance(plane, ray);
    
    return get_point(ray, t);
  }
}

// ======== Sphere =============================================================


vec3 nearest_point_on_sphere_unclamped(in Sphere sphere, in Ray ray)
{
  vec3 nearest_point = nearest_point_unclamped(ray, sphere.origin);

  float distance_to_ray = distance(sphere.origin, nearest_point);
  
  float d = min(distance_to_ray, sphere.radius);

  nearest_point = sphere.origin + d * (nearest_point - sphere.origin) / distance_to_ray;
  
  return nearest_point - ray.direction * sphere.radius * sin(acos(d/sphere.radius));
}

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

vec2 map_direction_to_rect_plane(in Rect rect, in vec3 direction)
{
  vec2 dir_2d = vec2(dot(direction-rect.origin, rect.tangent1),
                     dot(direction-rect.origin, rect.tangent2));
  
  // TODO potential performance issue
  return sq(dir_2d)!=0.f ? vec2(0) : normalize(dir_2d);
}

vec3 map_point_from_rect_plane(in Rect rect, in vec2 p)
{
  return rect.origin + p.x*rect.tangent1 + p.y*rect.tangent2;
}

vec2 clamp_point_to_rect(in vec2 half_size, in vec2 p)
{
  float dist = length(p); // TODO potential numeric issue. write unittest for this
  vec2 dir = p / dist;

  float t = min_component(abs(half_size / dir));

  p = dir * min(dist, t);

  return p;
}

vec2 clamp_point_to_rect(in Rect rect, in vec2 p)
{
  return clamp_point_to_rect(vec2(rect.half_width, rect.half_height), p);
}

vec3 clamp_point_to_rect(in Rect rect, in vec3 point)
{
  return map_point_from_rect_plane(rect, clamp_point_to_rect(rect, map_point_to_rect_plane(rect, point)));
}

// aabb.x the x coordinate of the edge with the lower x coordinate
// aabb.z the x coordinate of the edge with the larger x coordinate
// aabb.y the y coordinate of the edge with the lower y coordinate
// aabb.w the y coordinate of the edge with the larger y coordinate
vec4 intersection_distance_rect2d_aabb(in vec4 aabb, in vec2 ray_origin, in vec2 ray_direction)
{
  // https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
  // Formula for arbitrary 3d planes intersecting with 3d ray:
  // (plane.d - dot(ray.origin, plane.normal)) / dot(ray.direction, plane.normal);
  //
  // Formular for 2d plane with normal (1,0) with 2d ray:
  // (plane.d - ray.origin.x) / ray.direction.x;
  
  return (aabb - ray_origin.xyxy) / ray_direction.xyxy;
}

// aabb.x the x coordinate of the edge with the lower x coordinate
// aabb.z the x coordinate of the edge with the larger x coordinate
// aabb.y the y coordinate of the edge with the lower y coordinate
// aabb.w the y coordinate of the edge with the larger y coordinate
vec2 nearest_point_of_ray_in_rect_2d(in vec4 aabb, in vec2 ray_origin, in vec2 ray_direction)
{
  vec4 t = intersection_distance_rect2d_aabb(aabb, ray_origin, ray_direction); 
  bool inside = any(lessThan(t, vec4(0)));
  
  // TODO possible to improve performance by multiplying with float instead of using ?:
  vec2 intersection = sq(ray_direction)==0.f||inside ? ray_origin : ray_origin+ray_direction*min_component(t);
  
  return clamp_point_to_rect(aabb.zw, intersection);
}

bool nearest_point_on_rect(in Rect rect, in Ray ray, out(vec3) nearest_point)
{
  Plane plane = plane_from_rect(rect);
  vec3 plane_intersection_point;
  
  bool intersects = intersection_point(plane, ray, plane_intersection_point);
  
  nearest_point = clamp_point_to_rect(rect, plane_intersection_point);

  return intersects;
}

vec3 mrp(in Rect rect, in Ray ray)
{
  vec4 aabb = vec4(-rect.half_width, -rect.half_height, rect.half_width, rect.half_height);
  
  return map_point_from_rect_plane(rect, nearest_point_of_ray_in_rect_2d(aabb, map_point_to_rect_plane(rect, ray.origin), map_direction_to_rect_plane(rect, ray.direction)));
}

