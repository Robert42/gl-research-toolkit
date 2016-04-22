#version 450 core
#include "posteffect.fs.glsl"

vec3 rayMarch(in Ray ray, out vec4 color)
{
  float intersection_distance;
  if(!intersects_aabb(ray, vec3(0), vec3(1), intersection_distance))
    discard;
  color = vec4(0.5, 0.25, 0, 1);
  return get_point(ray, intersection_distance);
}