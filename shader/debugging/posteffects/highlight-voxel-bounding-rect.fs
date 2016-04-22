#version 450 core
#include "posteffect.fs.glsl"

vec3 rayMarch(in Ray ray, out vec4 color)
{
  if(!intersects_aabb(ray, vec3(0), vec3(1)))
    discard;
  color = vec4(0.5, 0.25, 0, 1);
  return vec3(0.f);
}