#version 450 core
#include "posteffect.fs.glsl"

float rayMarch(in Ray ray, out vec4 color)
{
  color = vec4(1, 0.5, 0, 1);
  return 1.f;
}