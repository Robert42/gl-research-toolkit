#ifndef _GLRT_GLSL_MATH_H_
#error Do not include this file directly, instead, include <glrt/glsl/math.h>
#endif

// ======== Structs ==========================================================


struct Sphere
{
  vec3 origin;
  float radius;
};

struct Disk
{
  vec3 origin;
  float radius;
  vec3 normal;
};

struct Rect
{
  vec3 origin;
  float half_width;
  vec3 tangent1;
  float half_height;
  vec3 tangent2;
};

struct Tube
{
  vec3 origin;
  float radius;
  vec3 direction;
  float length;
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


vec3 _most_representative_point_on_plane(in Plane plane, in vec3 origin_of_region_of_interest, in float radius_of_region_of_interrest, in Ray ray)
{
  float t;
  bool intersects = intersection_test(plane, ray, t);
  
  if(intersects)
  {
    return get_point(ray, t);
  }else
  {
    ray.direction -= dot(ray.direction, plane.normal);
    return origin_of_region_of_interest + normalize(ray.direction) * radius_of_region_of_interrest;
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

vec3 clamp_point_to_rect(in Rect rect, in vec3 point)
{
  float w = dot(point-rect.origin, rect.tangent1);
  float h = dot(point-rect.origin, rect.tangent2);

  w = clamp(w, -rect.half_width,  rect.half_width);
  h = clamp(h, -rect.half_height, rect.half_height);

  return rect.origin + w*rect.tangent1 + h*rect.tangent2;
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
  vec3 candidate = _most_representative_point_on_plane(plane_from_rect(rect), rect.origin, rect.half_width+rect.half_height, ray);
  
  return clamp_point_to_rect(rect, candidate);
}

