#ifndef _GLRT_GLSL_MATH_H_
#error Do not include this file directly, instead, include <glrt/glsl/math.h>
#endif


#include "./structs.h"

// ======== Ray ================================================================

inline Ray ray_from_two_points(vec3 from, vec3 to)
{
  Ray r;
  r.direction = normalize(to-from);
  r.origin = from;
  return r;
}

inline vec3 get_point(in Ray ray, float t)
{
  return ray.origin + ray.direction * t;
}

inline void get_points(in Ray ray, in vec3 t, out(vec3) p1, out(vec3) p2, out(vec3) p3)
{
  p1 = get_point(ray, t.x);
  p2 = get_point(ray, t.y);
  p3 = get_point(ray, t.z);
}

// ---- nearest_point

inline vec3 nearest_point(in Ray ray, in vec3 point)
{
  return ray.origin + ray.direction * max(0.f, dot(ray.direction, point-ray.origin));
}

inline vec3 nearest_point_unclamped(in Ray ray, in vec3 point)
{
  return ray.origin + ray.direction * dot(ray.direction, point-ray.origin);
}

inline vec3 nearest_point_clamped(in Ray ray, in vec3 point, float min, float max)
{
  return ray.origin + ray.direction * clamp(dot(ray.direction, point-ray.origin), min, max);
}

inline vec3 nearest_point_to_line_segment(vec3 from, vec3 to, in vec3 point)
{
  Ray r;
  r.direction = to-from;
  r.origin = from;
  float len = length(r.direction);
  r.direction /= len;
  
  return nearest_point_clamped(r, point, 0.f, len);
}

// ---- distance

inline float sq_distance_to_unclamped(in Ray ray, in vec3 point)
{
  return sq(nearest_point_unclamped(ray, point)-point);
}

inline float distance_to_unclamped(in Ray ray, in vec3 point)
{
  return distance(nearest_point_unclamped(ray, point), point);
}

inline float sq_distance_to(in Ray ray, in vec3 point)
{
  return sq(nearest_point(ray, point)-point);
}

inline float distance_to(in Ray ray, in vec3 point)
{
  return distance(nearest_point(ray, point), point);
}

// ---- intersection_distance

inline vec3 intersection_distance_to_axis_planes(in Ray ray, in vec3 common_point)
{
  return (common_point - ray.origin) / ray.direction;
}

inline vec3 intersection_distance_to_grid(in Ray ray)
{
  vec3 common_point = floor(ray.origin) + max(vec3(0), sign(ray.direction));

  return intersection_distance_to_axis_planes(ray, common_point);
}

inline vec3 intersection_distance_to_grid(in Ray ray, in vec3 aabbMin, in vec3 aabbMax)
{
  vec3 common_point = floor(ray.origin) + max(vec3(0), sign(ray.direction));

  common_point = clamp(common_point, aabbMin, aabbMax);

  return intersection_distance_to_axis_planes(ray, common_point);
}

// ---- contains

inline bool contains(in Ray ray, in vec3 point, float epsilon)
{
  return sq_distance_to(ray, point)<= sq(epsilon);
}

inline bool contains_unclamped(in Ray ray, in vec3 point, float epsilon)
{
  return sq_distance_to_unclamped(ray, point)<= sq(epsilon);
}

// ---- intersects

inline void __intersects_aabb_common_points(in Ray ray, in vec3 aabbMin, in vec3 aabbMax, out(vec3) frontFace, out(vec3) backFace)
{
  // Choose componentwise the nearer value
  vec3 use_for_frontface = max(vec3(0), sign(ray.origin - (aabbMin+aabbMax)*0.5f));
  vec3 use_for_backface = vec3(1) - use_for_frontface;
  
  frontFace = mix(aabbMin, aabbMax, use_for_frontface);
  backFace = mix(aabbMin, aabbMax, use_for_backface);
}

inline vec3 __intersects_aabb_intersection_with_common_point(in Ray ray, in vec3 aabbMin, in vec3 aabbMax, in vec3 common_point, out(bvec3) intersects)
{
  vec3 distances = intersection_distance_to_axis_planes(ray, common_point);
  vec3 p1;
  vec3 p2;
  vec3 p3;
  
  get_points(ray, distances, p1, p2, p3);

  intersects = bvec3((all(lessThanEqual(aabbMin.yz, p1.yz)) && all(lessThanEqual(p1.yz, aabbMax.yz))),
                     (all(lessThanEqual(aabbMin.xz, p2.xz)) && all(lessThanEqual(p2.xz, aabbMax.xz))),
                     (all(lessThanEqual(aabbMin.xy, p3.xy)) && all(lessThanEqual(p3.xy, aabbMax.xy))));
                           
  return distances;
}

inline bool aabb_contains(in vec3 pos, in vec3 aabbMin, in vec3 aabbMax)
{
  return all(lessThanEqual(aabbMin, pos)) && all(lessThan(pos, aabbMax));
}

// --

inline bool intersects_aabb(in Ray ray, in vec3 aabbMin, in vec3 aabbMax)
{
  vec3 front_face_common;
  vec3 back_face_common;
  __intersects_aabb_common_points(ray, aabbMin, aabbMax, front_face_common, back_face_common);

  bvec3 intersects;
  __intersects_aabb_intersection_with_common_point(ray, aabbMin, aabbMax, front_face_common, intersects);

  return any(intersects);
}

inline bool intersects_aabb(in Ray ray, in vec3 aabbMin, in vec3 aabbMax, out(float) intersection_distance, out(int) dimension)
{
  vec3 front_face_common;
  vec3 back_face_common;
  __intersects_aabb_common_points(ray, aabbMin, aabbMax, front_face_common, back_face_common);

  bvec3 intersects;
  vec3 distances = __intersects_aabb_intersection_with_common_point(ray, aabbMin, aabbMax, front_face_common, intersects);

  int i = index_of_min_component_masked(distances, intersects);
  dimension = i;
  intersection_distance = distances[i];
    
  return any(intersects);
}

inline bool intersects_aabb_twice(in Ray ray, in vec3 aabbMin, in vec3 aabbMax, out(float) intersection_distance_front, out(float) intersection_distance_back)
{
  bvec3 intersects1;
  bvec3 intersects2;
  vec3 distances1 = __intersects_aabb_intersection_with_common_point(ray, aabbMin, aabbMax, aabbMin, intersects1);
  vec3 distances2 = __intersects_aabb_intersection_with_common_point(ray, aabbMin, aabbMax, aabbMax, intersects2);
  
  intersection_distance_front = max(0.f, min(min_component_masked(distances1, intersects1), min_component_masked(distances2, intersects2)));
  intersection_distance_back  = max(0.f, max(max_component_masked(distances1, intersects1), max_component_masked(distances2, intersects2)));

  return any(intersects1) ||  any(intersects2);
}


// ======== Plane ==============================================================

// normal must have length 1
inline Plane plane_from_normal(in vec3 normal, in float d)
{
  Plane p;

  p.normal = normal;
  p.d = d;

  return p;
}

// normal must have length 1
inline Plane plane_from_normal(in vec3 normal, in vec3 point)
{
  return plane_from_normal(normal, dot(normal, point));
}

// normal will be normalized t1 x t2
inline Plane plane_from_tangents(in vec3 tangent1, in vec3 tangent2, in vec3 origin)
{
  return plane_from_normal(normalize(cross(tangent1, tangent2)),
                           origin);
}

// a,b,c ccw will have the normal point towards the viewer
inline Plane plane_from_three_points(in vec3 a, in vec3 b, in vec3 c)
{
  return plane_from_tangents(a-c, b-c, c);
}

inline Plane plane_from_rect(in Rect rect)
{
  return plane_from_tangents(rect.tangent1, rect.tangent2, rect.origin);
}

inline Plane plane_for_projection(in Ray ray, float image_plane_distance)
{
  return plane_from_normal(ray.direction, get_point(ray, image_plane_distance));
}

// ---- distances

// https://de.wikipedia.org/wiki/Hessesche_Normalform#Abstand_2
inline float signed_distance_to(in Plane plane, in vec3 point)
{
  return dot(plane.normal, point) - plane.d;
}

inline float distance_to(in Plane plane, in vec3 point)
{
  return abs(signed_distance_to(plane, point));
}

// ---- front/backside

inline bool is_on_frontside(in Plane plane, in vec3 point)
{
  return signed_distance_to(plane, point) > 0;
}

// ---- nearest point

inline vec3 nearest_point(in Plane plane, in vec3 point)
{
  return point - plane.normal * signed_distance_to(plane, point);
}

// ---- contains

inline bool contains(in Plane plane, in vec3 point, float epsilon)
{
  float signed_distance = signed_distance_to(plane, point);

  return abs(signed_distance) < epsilon;
}

// ---- intersection


// returns infinity, if there's no intersection at all
inline float intersection_distance(in Plane plane, in Ray ray)
{
  // https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
  return (plane.d - dot(ray.origin, plane.normal)) / dot(ray.direction, plane.normal);
}

inline bool intersection_test_unclamped(in Plane plane, in Ray ray, out(float) distance)
{
  distance = intersection_distance(plane, ray);
  
  return !isnan(distance);
}

inline bool intersection_test(in Plane plane, in Ray ray, out(float) distance)
{
  distance = intersection_distance(plane, ray);
  
  return distance >= 0 && !isnan(distance);
}

inline bool intersection_point_unclamped(in Plane plane, in Ray ray, out(vec3) point)
{
  float t;
  bool intersects = intersection_test_unclamped(plane, ray, t);
  
  point = get_point(ray, t);
  
  return intersects;
}

inline bool intersection_point(in Plane plane, in Ray ray, out(vec3) point)
{
  float t;
  bool intersects = intersection_test(plane, ray, t);

  point = get_point(ray, t);

  return intersects;
}

// TODO: what if point is equal wo view_position?
inline vec3 perspective_projection_unclamped(in Plane plane, in vec3 view_position, in vec3 point)
{
  Ray ray;
  ray.direction = normalize(point-view_position);
  ray.origin = view_position;
      
  vec3 intersection_point;
  intersection_point_unclamped(plane, ray, intersection_point);
  return intersection_point;
}


// ======== Sphere =============================================================

inline vec3 nearest_point_on_sphere_unclamped(in Sphere sphere, in Ray ray)
{
  vec3 nearest_point = nearest_point_unclamped(ray, sphere.origin);

  float distance_to_ray = distance(sphere.origin, nearest_point);
  
  float d = min(distance_to_ray, sphere.radius);

  nearest_point = sphere.origin + d * (nearest_point - sphere.origin) / distance_to_ray;
  
  return nearest_point - ray.direction * sphere.radius * sin(acos(d/sphere.radius));
}

inline bool intersects(in Sphere sphere, in Ray ray)
{
  float d = sq_distance_to(ray, sphere.origin);
  
  return d <= sq(sphere.radius);
}


// ======== Rect =============================================================

inline vec2 map_point_to_rect_plane(in Rect rect, in vec3 point)
{
  return vec2(dot(point-rect.origin, rect.tangent1),
              dot(point-rect.origin, rect.tangent2));
}

inline vec3 map_point_from_rect_plane(in Rect rect, in vec2 p)
{
  return rect.origin + p.x*rect.tangent1 + p.y*rect.tangent2;
}

inline vec2 clamp_point_to_rect(in vec2 half_size, in vec2 p)
{
  return clamp(p, -half_size, half_size);
}

inline vec2 clamp_point_to_rect(in Rect rect, in vec2 p)
{
  return clamp_point_to_rect(vec2(rect.half_width, rect.half_height), p);
}

inline vec3 clamp_point_to_rect(in Rect rect, in vec3 point)
{
  return map_point_from_rect_plane(rect, clamp_point_to_rect(rect, map_point_to_rect_plane(rect, point)));
}

inline bool contains_mapped_point(in Rect rect, in vec2 p)
{
  return p.x >= -rect.half_width && p.y >= -rect.half_height && p.x <= rect.half_width && p.y <= rect.half_height;
}

inline bool contains_mapped_point(in Rect rect, in vec3 p)
{
  return contains_mapped_point(rect, map_point_to_rect_plane(rect, p));
}

inline bool intersects_unclamped(in Rect rect, in Ray ray)
{
  Plane plane = plane_from_rect(rect);
  
  vec3 p;
  return intersection_point_unclamped(plane, ray, p) && contains_mapped_point(rect, p);
}


// ======== Triangles =============================================================

// returns 0 for no intersection, 1 for an intersection from the frontside and -1 for intersecting from the backside
inline bool triangle_ray_intersection_unclamped(in Ray ray, in vec3 v0, in vec3 v1, in vec3 v2, float treshold=1.e-9f)
{
  Plane plane = plane_from_three_points(v0, v1, v2);
  vec3 p;
  
  bool intersectsPlane = intersection_point_unclamped(plane, ray, p);
  
  vec3 uvw;
  vec3 closestPoint = closestPointOnTriangleToPoint(v0, v1, v2, p, uvw);
  
  return intersectsPlane && sq_distance(closestPoint, p)<=treshold;
}


// ======== Cone ===============================================================

inline Cone cone_from_ray_angle(in vec3 origin, in vec3 direction, float half_cone_angle)
{
  Cone cone;
  cone.origin = origin;
  cone.direction = direction;
  cone.inv_cos_half_angle = 1.f / cos(half_cone_angle);
  cone.tan_half_angle = tan(half_cone_angle);
  return cone;
}

inline Cone cone_from_ray_tan_angle(in vec3 origin, in vec3 direction, float tan_half_cone_angle)
{
  Cone cone;
  cone.origin = origin;
  cone.direction = direction;
  cone.inv_cos_half_angle = 1.f / cos(atan(tan_half_cone_angle));
  cone.tan_half_angle = tan_half_cone_angle;
  return cone;
}

inline Cone cone_from_ray_angle(in Ray ray, float half_cone_angle)
{
  return cone_from_ray_angle(ray.origin, ray.direction, half_cone_angle);
}

inline Ray ray_from_cone(in Cone cone)
{
  Ray ray;
  ray.origin = cone.origin;
  ray.direction = cone.direction;
  return ray;
}

inline float cone_half_angle(in Cone cone)
{
  return atan(cone.tan_half_angle);
}

// cos(radians(89.f)) \equiv 0.99985f
inline Cone cone_from_point_to_sphere(vec3 origin, in Sphere sphere, float max_sin = 0.99985f)
{
  vec3 direction = sphere.origin - origin;
  
  const float inv_distance = 1.f / length(direction);
  direction *= inv_distance;

  const float sin_of_angle = inv_distance * sphere.radius;
  const float half_angle = asin(min(max_sin, sin_of_angle));
  
  return cone_from_ray_angle(origin, direction, half_angle);
}

inline bool cone_intersects_sphere(in Cone cone, Sphere sphere, out(float) t)
{
  t = dot(cone.direction, sphere.origin-cone.origin);
  const float clamped_t = max(0.f, t);
  
  const vec3 p = cone.origin + cone.direction * clamped_t;

  // see doc/cone_intersects_sphere.svg
  // for t>0: the nearest point on the center ray of the cone to the sphere
  // origin is not the nearest point on the cone to the sphere. use inv_cos_half_angle for correction (see doc/cone_intersects_sphere.svg)
  const float corrected_min_distance_to_sphere_origin = sphere.radius * cone.inv_cos_half_angle + cone.tan_half_angle * clamped_t;
  // For t <=0 just check, that the sphere center is close eneough to the cone origin
  const float clamped_min_distance_to_sphere_origin = sphere.radius;

  // equivalent to t<0.f ? clamped_min_distance_to_sphere_origin : corrected_min_distance_to_sphere_origin
  const float min_distance_to_sphere_origin = mix(clamped_min_distance_to_sphere_origin, corrected_min_distance_to_sphere_origin, step(0.f, t));

  const float d = distance(p, sphere.origin);

  return d <= min_distance_to_sphere_origin;
}

inline bool cone_intersects_sphere(in Cone cone, Sphere sphere)
{
  float t;
  return cone_intersects_sphere(cone, sphere, t);
}
