#version 450 core

#include "debugging.glsl"

in vec3 fragment_color;
in vec3 fragment_normal;

out vec4 color;

void main()
{
  float light;

  if(u.debuggingMode == DebuggingNone)
    light = max(0, dot(normalize(fragment_normal), -u.light_direction));
  else
    light = 1.;

  color = vec4(fragment_color*light, 1);
}
