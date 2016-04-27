#version 450 core
#include "posteffect.fs.glsl"

layout(std140, binding=UNIFORM_BINDING_DEBUG_POSTEFFECT_FRAGMENT) uniform PostEffectBlock
{
  Sphere orangeSphere;
};

void rayMarch(in Ray ray, out vec4 color, out vec3 world_pos, out vec3 world_normal)
{
  if(!intersects(orangeSphere, ray))
    discard;
  color = vec4(1, 0.5, 0, 1);
  world_pos = nearest_point_on_sphere_unclamped(orangeSphere, ray);
  world_normal = normalize(world_pos - orangeSphere.origin);
}