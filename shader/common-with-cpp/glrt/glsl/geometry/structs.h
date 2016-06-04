#ifndef _GLRT_GLSL_GEOMETRY_STRUCTS_H_
#define _GLRT_GLSL_GEOMETRY_STRUCTS_H_

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

struct Cone
{
  vec3 origin;
  float tan_half_angle;
  vec3 direction;
  float inv_cos_half_angle;
};

#endif // _GLRT_GLSL_GEOMETRY_STRUCTS_H_
