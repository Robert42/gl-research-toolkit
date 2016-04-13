#version 450 core
#include "posteffect.fs.glsl"

layout(std140, binding=UNIFORM_BINDING_DEBUG_POSTEFFECT_FRAGMENT) uniform PostEffectBlock
{
  Sphere orangeSphere;
};

vec3 rayMarch(in Ray ray, out vec4 color)
{
  if(!intersects(orangeSphere, ray))
    discard;
  color = vec4(1, 0.5, 0, 1);
  return nearest_point_on_sphere_unclamped(orangeSphere, ray);
}